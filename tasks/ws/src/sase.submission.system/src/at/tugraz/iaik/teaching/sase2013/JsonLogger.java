/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;

import com.fasterxml.jackson.core.JsonFactory;
import com.fasterxml.jackson.core.JsonGenerator;

/**
 * A simple logger that logs messages in an in memory buffer using a JSON based
 * output format. The JSON format is as follows:
 * <code>{"Log":[msg1, msg2, ..., msgn], "ExitCode":exitCode}</code>
 * 
 * @author SASE 2013 Team
 */
public class JsonLogger {

	/**
	 * Container class for JSON field names.
	 * 
	 * @author SASE 2013 Team
	 */
	private static class Json {
		public static final String LOG = "Log";
		public static final String EXIT_CODE = "ExitCode";
	}

	/**
	 * The JSON factory used to create the JSON generators. Here we actually want
	 * the generator's default behaviour, which closes the underlying output
	 * stream on closing the JSON generator.
	 */
	private final static JsonFactory JSON_FACTORY = new JsonFactory();

	private final StringWriter log;
	private final PrintWriter out;
	private final JsonGenerator g;

	private transient volatile boolean closed;

	/**
	 * Creates a new <code>JsonLogger</code>. This opens up an in memory buffer to
	 * hold the log entries.
	 * 
	 * @throws IOException
	 *           if creation of the in memory buffer fails
	 */
	public JsonLogger() throws IOException {
		this.log = new StringWriter();
		this.out = new PrintWriter(log);
		this.g = JSON_FACTORY.createGenerator(out);
		g.writeStartObject();
		g.writeFieldName(Json.LOG);
		g.writeStartArray();
		this.closed = false;
	}

	/**
	 * Log a message. The message is encapsulated into a JSON construct and stored
	 * in an in-memory buffer.
	 * 
	 * @param msg
	 *          the message to log
	 * @throws IOException
	 *           if writing to the in-memory buffer fails.
	 */
	public synchronized void log(String msg) throws IOException {
		if (closed) {
			return;
		}
		g.writeString(msg);
	}

	/**
	 * Closes the in-memory log buffer and the JSON generator writing to it.
	 * Returns the content of the in-memory buffer as a string. The exit code is
	 * only used the first time this method is called. Afterwards it is ignored.
	 * 
	 * @return the content of the in-memory log buffer as a JSON formatted string
	 * @param exitCode
	 *          the exit code of the command, the output of which has been logged
	 * @throws IOException
	 *           if closing of the in-memory buffer, or the JSON generator fails
	 */
	public synchronized String close(int exitCode) throws IOException {
		if (!closed) {
			closed = true;
			g.writeEndArray();
			g.writeNumberField(Json.EXIT_CODE, exitCode);
			g.writeEndObject();
			g.close();
			out.flush();
			out.close();
		}
		return log.toString();
	}

}
