/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

/**
 * Executes the program specified by the given command line and logs the output
 * of the program to a <code>JsonLogger</code>. This class is used to execute
 * the compiler for the uploaded student programs and to log the results of the
 * compilation in a format that is easily understood by the web client.
 * 
 * @author SASE 2013 Team
 */
public class Compiler {

	/**
	 * To prevent an external process from locking up, if either the standard out
	 * or standard err buffer are filled up, we use two threads to read from both
	 * streams concurrently. These threads use the InputStreamLogger
	 * {@link Runnable} to implement reading from a process input stream and log
	 * the output to a given {@link JsonLogger}.
	 * 
	 * @author SASE 2013 Team
	 */
	private class InputStreamLogger implements Runnable {

		private final BufferedReader in;
		private final JsonLogger logger;

		private volatile boolean finished;

		/**
		 * Creates a new InputStreamLogger with the given InputStream to read from
		 * and the given {@link JsonLogger} to log to.
		 * 
		 * @param in the InputStream to read from.
		 * @param logger the JsonLogger to log to.
		 */
		public InputStreamLogger(InputStream in, JsonLogger logger) {
			super();
			this.in = new BufferedReader(new InputStreamReader(in));
			this.logger = logger;
			this.finished = false;
		}

		/*
		 * (non-Javadoc)
		 * 
		 * @see java.lang.Runnable#run()
		 */
		@Override
		public void run() {
			try {
				for (String line = null; (line = in.readLine()) != null;) {
					logger.log(line);
				}
			} catch (IOException e) {
			}
			synchronized (this) {
				finished = true;
				this.notifyAll();
			}
		}

		/**
		 * Waits for the run method to finish.
		 * @throws InterruptedException if the thread waiting gets interrupted
		 */
		public synchronized void waitFor() throws InterruptedException {
			while (!finished) {
				wait();
			}
		}

	}

	private final static Logger log = LogManager
	    .getLogger(SaseSubmissionSystem.LOGGER_ID);

	private final JsonLogger jsonLogger;
	private final String cmd;
	private final File dir;

	/**
	 * Creates a new <code>Compiler</code> object with the given command to
	 * execute, the given working directory for the command and a
	 * <code>JsonLogger</code> to log the command output to.
	 * 
	 * @param cmd
	 *          the command to execute
	 * @param dir
	 *          the working directory for the command to execute in
	 * @param jsonLogger
	 *          the <cod>JsonLogger</code> for the command output
	 */
	public Compiler(String cmd, File dir, JsonLogger jsonLogger) {
		super();
		this.jsonLogger = jsonLogger;
		this.cmd = cmd;
		this.dir = dir;
	}

	/**
	 * Executes the command in the given working directory and logs the output to
	 * the <code>JsonLogger</code>.
	 * 
	 * @returns the log as JSON encoded string
	 * @throws IOException
	 *           if logging, or reading the command output fails.
	 */
	public String compile() throws IOException {
		log.entry();
		Runtime rt = Runtime.getRuntime();
		Process p = rt.exec(cmd, null, dir);
		InputStreamLogger inputStreamLogger = new InputStreamLogger(
		    p.getInputStream(), jsonLogger);
		InputStreamLogger errStreamLogger = new InputStreamLogger(
		    p.getErrorStream(), jsonLogger);
		new Thread(inputStreamLogger).start();
		new Thread(errStreamLogger).start();
		try {
			p.waitFor();
			inputStreamLogger.waitFor();
			errStreamLogger.waitFor();
		} catch (InterruptedException e) {
			throw new IOException(e);
		}

		String report = jsonLogger.close(p.exitValue());
		log.exit();
		return report;
	}
}
