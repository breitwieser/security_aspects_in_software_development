package at.tugraz.iaik.teaching.sase2013;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import at.tugraz.iaik.teaching.sase2013.db.DatabaseOperation;
import at.tugraz.iaik.teaching.sase2013.db.DatabaseOperationExecutor;
import at.tugraz.iaik.teaching.sase2013.db.DeleteUser;
import at.tugraz.iaik.teaching.sase2013.db.UpdateAdminConfig;

/**
 * Implements administration component controlling functions. These are all
 * functions that are only available from the administration realm. The
 * functions supported by this controller are:
 * <ul>
 * <li>Updating an SASE SUB SYSTEM wide configuration property, such as the
 * command line of the compiler used to compile student submission.</li>
 * <li>Deleting a user from the SASE SUB SYSTEM.
 * </ul>
 */
@WebServlet(description = "Handles all incoming tasks in the administration realm.", urlPatterns = {
    "/admin/config_edit.do", "/admin/user_del.do" })
public class AdminController extends GenericController {
	private static final long serialVersionUID = 1L;

	private final Logger log = LogManager
	    .getLogger(SaseSubmissionSystem.LOGGER_ID);
	private final static DatabaseOperationExecutor exec = DatabaseOperationExecutor
	    .getInstance();

	/**
	 * @see HttpServlet#HttpServlet()
	 */
	public AdminController() {
		super();
	}

	/**
	 * Update a SASE SUB SYSTEM wide configuration property. The method reads two
	 * request parameters the <code>'id'</code> of the value to change and the new
	 * <code>'value'</code>.
	 * 
	 * @param request
	 *          the HTTP servlet request
	 * @param response
	 *          the HTTP servlet response
	 * @throws ServletException
	 *           if updating the configuration property fails, see
	 *           {@link UpdateAdminConfig} for details
	 * @throws IOException
	 *           if an I/O problem occurs
	 */
	protected void configurationEdit(HttpServletRequest request,
	    HttpServletResponse response) throws ServletException, IOException {
		log.entry();
		String idParam = request.getParameter("id");
		String valueParam = request.getParameter("value");
		DatabaseOperation op = new UpdateAdminConfig(Integer.parseInt(idParam
		    .trim()), valueParam.trim());
		exec.executeDbOperation(op);
		log.exit();
	}

	/**
	 * Deletes a user with a specific id from the database. The method reads the
	 * <code>'id'</code> request parameter and deletes the user with this id, if
	 * she exists. If the user does not exist this method will do nothing.
	 * 
	 * @param request
	 *          the HTTP servlet request
	 * @param response
	 *          the HTTP servlet response
	 * @throws ServletException
	 *           if deleting the user fails. See {@link DeleteUser} for details.
	 * @throws IOException
	 *           if an I/O problem occurs.
	 */
	protected void userDelete(HttpServletRequest request,
	    HttpServletResponse response) throws ServletException, IOException {
		log.entry();
		String idParam = request.getParameter("id");
		DatabaseOperation op = new DeleteUser(Integer.parseInt(idParam.trim()));
		exec.executeDbOperation(op);
		// TODO Logout deleted user, if she is logged in.
		log.exit();
	}

	/*
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse
	 * response)
	 */
	@Override
	protected void doPost(HttpServletRequest request, HttpServletResponse response)
	    throws ServletException, IOException {
		AppResource r = mapServletPathToResource(request);
		switch (r) {
		case ADMIN_UPDATE_CONFIG:
			configurationEdit(request, response);
			break;
		case ADMIN_DELETE_USER:
			userDelete(request, response);
			break;
		default:
			throw new ServletException(generateUnmappedErrorMessage(request));

		}
	}

}
