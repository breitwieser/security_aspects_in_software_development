package at.tugraz.iaik.teaching.sase2013;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.MessageFormat;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import javax.servlet.ServletException;
import javax.servlet.http.Part;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

/**
 * Unzips an uploaded file into a directory. 
 * @author SASE 2013 Team
 */
public class Unzip {

	private final static Logger log = LogManager
	    .getLogger(SaseSubmissionSystem.LOGGER_ID);

	private final Part zippy;
	private final File dir;
	private final JsonLogger jsonLog;

	/**
	 * Creates a new <code>Unzip</code> object with the given file upload part,
	 * target directory, and JSON log.
	 * 
	 * @param zippy
	 *          the uploaded zip file as a part of a multipart encoded upload
	 * @param dir
	 *          the target directory
	 * @param jsonLog
	 *          the JSON log to report the unzip status to
	 */
	public Unzip(Part zippy, File dir, JsonLogger jsonLog) {
		super();
		this.zippy = zippy;
		this.dir = dir;
		this.jsonLog = jsonLog;
	}

	/**
	 * Unzips a single zip file entry into the given file
	 * 
	 * @param zin
	 *          the {@link ZipInputStream} to read from
	 * @param f
	 *          the file to write to
	 * @throws IOException
	 *           if an I/O problem occurs while reading from the ZIP file, or
	 *           writing to the output file
	 */
	private void writeEntry(ZipInputStream zin, File f) throws IOException {
		log.entry();
		BufferedOutputStream fout = new BufferedOutputStream(
		    new FileOutputStream(f));
		try {
			int read = 0;
			byte[] buffer = new byte[4096];
			while ((read = zin.read(buffer, 0, buffer.length)) != -1) {
				fout.write(buffer, 0, read);
			}
			jsonLog.log(MessageFormat.format("File \"{0}\" written.",
			    f.getAbsolutePath()));
		} finally {
			fout.close();
		}
		log.exit();
	}

	/**
	 * Unzips the given zip file into the specified target directory.
	 * 
	 * @throws IOException
	 *           if an I/O exception occurs while reading the zip file, or writing
	 *           the zip entry to the target directory.
	 * @throws ServletException
	 *           if the zip file contains a directory and this directory cannot be
	 *           created below the target directory hiearachy
	 */
	public void doUnzip() throws IOException, ServletException {
		log.entry();
		ZipEntry entry = null;
		ZipInputStream zin = new ZipInputStream(zippy.getInputStream());
		try {
			while ((entry = zin.getNextEntry()) != null) {
				File f = new File(dir, entry.getName());
				if (entry.isDirectory()) {
					jsonLog.log(MessageFormat.format(
					    "Creating directory entry \"{0}\" here: \"{1}\".",
					    entry.getName(), f.getAbsolutePath()));
					if (!f.mkdir()) {
						throw new ServletException(MessageFormat.format(
						    "Cannot create directory {0}.", f.getAbsolutePath()));
					}
				} else {
					jsonLog.log(MessageFormat.format(
					    "Writing ZIP entry \"{0}\" to file \"{1}\".", entry.getName(),
					    f.getAbsolutePath()));
					writeEntry(zin, f);
				}
			}
		} finally {
			zin.close();
		}
		log.exit();
	}
}
