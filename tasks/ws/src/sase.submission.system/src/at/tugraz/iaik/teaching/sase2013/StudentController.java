package at.tugraz.iaik.teaching.sase2013;

import java.io.File;
import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.annotation.MultipartConfig;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.Part;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import at.tugraz.iaik.teaching.sase2013.db.DatabaseOperationExecutor;
import at.tugraz.iaik.teaching.sase2013.db.GetCompilerCommand;
import at.tugraz.iaik.teaching.sase2013.db.ResultDatabaseOperation;

/**
 * Implements Student component controlling functions. These are all the
 * functions that are only available from the students realm (/submission). The
 * functions supported by this controller are:
 * <ul>
 * <li>Uploading, decompressing and compiling of a user program.</li>
 * </ul>
 * 
 * @author The SASE2013 Team
 */
@WebServlet("/submission/upload.do")
@MultipartConfig
public class StudentController extends GenericController {

	private static final DatabaseOperationExecutor exec = DatabaseOperationExecutor
	    .getInstance();

	private final static Logger log = LogManager
	    .getLogger(SaseSubmissionSystem.LOGGER_ID);

	private static final long serialVersionUID = 1L;

	/**
	 * Receives an uploaded zip file containing the users program, decompresses
	 * it, and compiles the content using the compiler command specified in the
	 * global SASE SUB SYSTEM configuration.
	 * 
	 * @param request
	 *          the HTTP servlet request
	 * @param response
	 *          the HTTP servlet response
	 * @throws ServletException
	 *           if receiving or compiling the program fails
	 * @throws IOException
	 *           if unzipping of the program fails, or another I/O problem occurs
	 */
	protected void upload(final HttpServletRequest request,
	    final HttpServletResponse response) throws ServletException, IOException {
		log.entry();
		JsonLogger jsonLogger = new JsonLogger();
		Part zipInput = request.getPart("submission");
		File tmpDir = Utils.createTemporaryDirectory();
		try {
			Unzip zipper = new Unzip(zipInput, tmpDir, jsonLogger);
			zipper.doUnzip();
			ResultDatabaseOperation rop = new GetCompilerCommand();
			exec.executeDbOperation(rop);
			Compiler c = new Compiler((String) rop.getResult(), tmpDir, jsonLogger);
			response.getWriter().println(c.compile());
		} finally {
			tmpDir.delete();
		}
		log.exit();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * javax.servlet.http.HttpServlet#doPost(javax.servlet.http.HttpServletRequest
	 * , javax.servlet.http.HttpServletResponse)
	 */
	protected void doPost(HttpServletRequest request, HttpServletResponse response)
	    throws ServletException, IOException {
		AppResource r = mapServletPathToResource(request);
		switch (r) {
		case SUBMISSION_DO_UPLOAD:
			upload(request, response);
			break;

		default:
			throw new ServletException(generateUnmappedErrorMessage(request));

		}
	}

}
