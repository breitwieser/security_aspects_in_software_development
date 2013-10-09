/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.security.Principal;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import static at.tugraz.iaik.teaching.sase2013.SaseSubmissionSystem.SessionAttribute;

/**
 * A set of utility functions used by the SASE SUBMISSION SYSTEM.
 * 
 * @author SASE 2013 Team
 */
public class Utils {

	private final static Logger log = LogManager
	    .getLogger(SaseSubmissionSystem.LOGGER_ID);

	/**
	 * Checks if the given string is either null, truly empty, or contains just
	 * whitespaces.
	 * 
	 * @param str
	 *          the string to check for emptiness
	 * @return true if the the string is either null, has a zero length or just
	 *         contains whitespaces
	 */
	public static boolean isStringEmpty(final String str) {
		return str == null || str.trim().isEmpty();
	}

	/**
	 * Retrieves a User object from the given HTTP Session. If no user object
	 * exits, and the session is not for authenticated user, this method returns
	 * null. If no user object exists, but the session is for an authenticated
	 * user, a user object is created an the back-end database is queried for the
	 * information of the user.
	 * 
	 * @param request
	 *          the HTTP request which contains the session to check for the user
	 *          object
	 * @return the user object if the session is authenticated; null otherwise
	 * @throws IOException
	 *           if an I/O problem occurs
	 * @throws ServletException
	 *           if a servlet specific problem occurs, or a database connection
	 *           problem occurs
	 */
	public static User getUser(final HttpServletRequest request)
	    throws ServletException, IOException {
		final Principal userPrincipal = request.getUserPrincipal();
		if (userPrincipal == null)
			return null;
		Object check = request.getSession().getAttribute(
		    SessionAttribute.USER_OBJECT_ID);
		if (!(check instanceof User)) {
			check = null;
		}
		if (check != null) {
			return (User) check;
		}
		User user = User.loadUser(userPrincipal.getName());
		request.getSession().setAttribute(SessionAttribute.USER_OBJECT_ID, user);
		return user;
	}

	/**
	 * Retrieves a User object from the given HTTP Session. If no user object
	 * exits, this method returns null.
	 * 
	 * @param request
	 *          the HTTP request which contains the session to check for the user
	 *          object
	 * @return the user object if the session is authenticated; null otherwise
	 */
	public static User getUserFromSession(final HttpServletRequest request) {
		Object check = request.getSession().getAttribute(
		    SessionAttribute.USER_OBJECT_ID);
		if (!(check instanceof User)) {
			return null;
		}
		return (User) check;
	}

	/**
	 * Determines the current display index, by trying to retrieve the value from
	 * the client request. If the client request does not specify a value, 1 is
	 * returned. Starting the index with 1 is due to the numbering used by SQL
	 * tables, which start to count with 1.
	 * 
	 * @param request
	 *          the HTTP servlet request to check for the index parameter
	 * @return the index parameter if present and in the correct format (int > 0);
	 *         1 otherwise
	 */
	public static int getDisplayIndex(final HttpServletRequest request) {
		int index = 1;
		String indexParameter = request
		    .getParameter(SaseSubmissionSystem.INDEX_PARAMETER_NAME);
		if (!isStringEmpty(indexParameter)) {
			try {
				index = Integer.parseInt(indexParameter);
				index = (index < 1) ? 1 : index;
			} catch (NumberFormatException e) {
				log.info("Invalid index: {}.", indexParameter, e);
			}
		}
		return index;
	}

	/**
	 * Dispatches a message to the client. This method uses the servlet's request
	 * dispatcher to forward the message to the given target resource. No further
	 * processing in the original servlet should be done after calling this
	 * method.
	 * 
	 * @param msg
	 *          the message to the client
	 * @param request
	 *          the request
	 * @param response
	 *          the response
	 * @param dest
	 *          the destination of the dispatch
	 * @throws ServletException
	 *           if a servlet specific problem occurs
	 * @throws IOException
	 *           if an I/O problem occurs
	 */
	public static void dispatchCommunique(String msg, HttpServletRequest request,
	    HttpServletResponse response, AppResource dest) throws ServletException,
	    IOException {
		ErrorCommunique e = new ErrorCommunique(msg, null);
		e.registerCommunique(request.getSession());
		RequestDispatcher rd = request.getRequestDispatcher(dest.getAppPath());
		rd.forward(request, response);
	}

	/**
	 * Dispatches a message + exception to the client. This method uses the
	 * servlet's request dispatcher to forward the message to the given target
	 * resource. No further processing in the original servlet should be done
	 * after calling this method.
	 * 
	 * @param msg
	 *          the message to the client
	 * @param t
	 *          the throwable corresponding to the message
	 * @param request
	 *          the request
	 * @param response
	 *          the response
	 * @param dest
	 *          the destination of the dispatch
	 * @throws ServletException
	 *           if a servlet specific problem occurs
	 * @throws IOException
	 *           if an I/O problem occurs
	 */
	public static void dispatchCommunique(String msg, Throwable t,
	    HttpServletRequest request, HttpServletResponse response, AppResource dest)
	    throws ServletException, IOException {
		ErrorCommunique e = new ErrorCommunique(msg, t);
		e.registerCommunique(request.getSession());
		RequestDispatcher rd = request.getRequestDispatcher(dest.getAppPath());
		rd.forward(request, response);
	}

	/**
	 * Returns a temporary directory created using the *NIX <code>mktemp</code>
	 * command. This is very expensive compared to the Java variant, but
	 * guaranteed to be safe. The temporary directory prefix is "SASE_SUB_SYS.".
	 * 
	 * @return a temporary directory, or null if the creation fails.
	 */
	public static File createTemporaryDirectoryWithMkTemp() {
		log.entry();
		ProcessBuilder pb = new ProcessBuilder("mktemp", "-d", "--tempdir",
		    SaseSubmissionSystem.APP_TEMP_DIR_PREFIX + "XXXXXXXXXX");
		try {
			Process p = pb.start();
			BufferedReader in = new BufferedReader(new InputStreamReader(
			    p.getInputStream()));
			BufferedReader ein = new BufferedReader(new InputStreamReader(
			    p.getErrorStream()));
			String dir = in.readLine();
			for (String line = null; (line = in.readLine()) != null;) {
				log.error(
				    "Warning, unexpected output by command: \"{}\". Output: \"{}\"",
				    pb.command(), line);
			}
			for (String line = null; (line = ein.readLine()) != null;) {
				log.warn(
				    "Error generating temporary directory. Command: \"{}\". Error output: \"{}\".",
				    pb.command(), line);
			}
			p.waitFor();
			if (p.exitValue() == 0) {
				log.exit();
				return new File(dir);
			}
		} catch (IOException e) {
			log.warn(
			    "Error generating temporary directory. Command: \"{}\". Exception\"{}\".",
			    pb.command(), e);
		} catch (InterruptedException e) {
			log.warn(
			    "Interuption while generating temporary directory. Command: \"{}\". Exception\"{}\".",
			    pb.command(), e);
		}
		log.exit();
		return null;
	}

	/**
	 * Tries to create a temporary directory in the system's temporary directory.
	 * The temporary directory is called
	 * <code>SASE_SUB_SYS.<i>Current Time in Milliseconds</i></code> This method
	 * uses the current time in milliseconds to distinguish the temporary
	 * directory. Theoretically, it could happen that a second process creates a
	 * directory with the same name at the same time. This would cause this method
	 * to fail and return null.
	 * 
	 * @return a temporary directory, or null if the creation fails.
	 */
	public static File createTemporaryDirectoryUsingJava() {
		log.entry();
		String name = SaseSubmissionSystem.APP_TEMP_DIR_PREFIX
		    + System.currentTimeMillis();
		File tmpDir = new File(SaseSubmissionSystem.SYSTEM_TEMP_DIR, name);
		if (tmpDir.mkdirs()) {
			log.exit();
			return tmpDir;
		} else {
			log.error(
			    "Warning, potential temporary directory creation race condition. Directory: \"{}\".",
			    tmpDir.getAbsolutePath());
			log.exit();
			return null;
		}
	}

	/**
	 * Returns a temporary directory, or null if temporary directory creation
	 * fails. This is convenience method that first tries to create a new
	 * temporary directory using the *NIX <code>mktemp</code> program. If that
	 * fails, it proceeds to create a temporary directory using Java.
	 * 
	 * @return a temporary directory, or null if the creation fails.
	 */
	public static File createTemporaryDirectory() {
		log.entry();
		File tempDir = createTemporaryDirectoryWithMkTemp();
		if (tempDir != null && tempDir.isDirectory()) {
			log.exit();
			return tempDir;
		}
		log.exit();
		return createTemporaryDirectoryUsingJava();
	}

}
