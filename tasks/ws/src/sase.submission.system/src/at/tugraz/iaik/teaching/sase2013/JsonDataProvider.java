package at.tugraz.iaik.teaching.sase2013;

import java.io.IOException;
import java.text.MessageFormat;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import at.tugraz.iaik.teaching.sase2013.User.AdminFacet;
import at.tugraz.iaik.teaching.sase2013.db.DatabaseOperation;
import at.tugraz.iaik.teaching.sase2013.db.DatabaseOperationExecutor;
import at.tugraz.iaik.teaching.sase2013.db.GetCondiments;
import at.tugraz.iaik.teaching.sase2013.db.ListAdminConfiguration;
import at.tugraz.iaik.teaching.sase2013.db.ListUsers;

import com.fasterxml.jackson.core.JsonGenerator;

/**
 * Servlet providing JSON formatted data in the ROOT application realm.
 * Specifically the Servlet generates:
 * <ul>
 * <li>User list: Generate a list of registered users. This function is only
 * available to administrators</li>
 * <li>Global configuration list: Generate a list of global configuration
 * parameters. This function is only available to administrators.</li>
 * <li>Login condiments: Provides the salt and the number of #iterations
 * required to digest the password for existing users.</li>
 * </ul>
 * 
 * @author SASE 2013 Team
 */
@WebServlet(description = "Provides all data relevant to the SASE SUB SYSTEM user interface via JSON", urlPatterns = { "*.json" })
public class JsonDataProvider extends GenericController {

	private static final long serialVersionUID = 1L;
	private final static Logger log = LogManager
	    .getLogger(SaseSubmissionSystem.LOGGER_ID);

	private final static DatabaseOperationExecutor exec = DatabaseOperationExecutor
	    .getInstance();

	/**
	 * Checks if the given user is an administrator by trying to access her
	 * administrator facet. If the user class fails to find the facet it will
	 * throw an exception. Thus, if the user is not logged in or not an
	 * administrator, this method will throw an exception.
	 * 
	 * @param admin
	 *          the user to check for administration privileges
	 * @return an {@link AdminFacet} if the user is an administrator
	 * @throws ServletException
	 *           if the user is no administrator, or loading the administration
	 *           facet from the database fails.
	 * @throws IOException
	 *           if an I/O problem occurs
	 */
	private AdminFacet checkAdminAccess(User admin) throws ServletException,
	    IOException {
		AdminFacet facet = null;
		if (admin == null || (facet = admin.getAdmin()) == null) {
			throw new ServletException(MessageFormat.format(
			    "You are not authorized to access resource \"{0}\".",
			    AppResource.ADMIN_USERS));
		}
		return facet;
	}

	/**
	 * Sends the list of registered users as JSON encoded string. This function is
	 * only available to administrators.
	 * 
	 * @param request
	 *          the HTTP servlet request
	 * @param response
	 *          the HTTP servlet response
	 * @throws ServletException
	 *           if the user is no administrator, or a database access error
	 *           occurs
	 * @throws IOException
	 *           if an I/O problem occurs
	 */
	private void printAdminUserList(final HttpServletRequest request,
	    final HttpServletResponse response) throws ServletException, IOException {
		log.entry();
		User admin = Utils.getUser(request);
		// check if user is admin
		AdminFacet facet = checkAdminAccess(admin);
		int index = Utils.getDisplayIndex(request);
		JsonGenerator g = SaseSubmissionSystem.JSON_FACTORY
		    .createGenerator(response.getWriter());
		DatabaseOperation op = new ListUsers(g, index, facet.getUserTableRows());
		exec.executeDbOperation(op);
		g.close();
		log.exit();
	}

	/**
	 * Sends the global configuration of the SASE SUB SYSTEM as JSON encoded
	 * string. This function is only available to administrators.
	 * 
	 * @param request
	 *          the HTTP servlet request
	 * @param response
	 *          the HTTP servlet response
	 * @throws ServletException
	 *           if the user is no administrator, or a database access error
	 *           occurs
	 * @throws IOException
	 *           if an I/O problem occurs
	 */
	private void printAdminConfiguration(final HttpServletRequest request,
	    final HttpServletResponse response) throws ServletException, IOException {
		log.entry();
		User admin = Utils.getUser(request);
		// check if user is admin
		AdminFacet facet = checkAdminAccess(admin);
		int index = Utils.getDisplayIndex(request);
		JsonGenerator g = SaseSubmissionSystem.JSON_FACTORY
		    .createGenerator(response.getWriter());
		try {
			DatabaseOperation op = new ListAdminConfiguration(g, index,
			    facet.getConfigTableRows());
			exec.executeDbOperation(op);
		} finally {
			g.close();
		}
		log.exit();
	}

	/**
	 * Sends the salt and the number of iterations required to digest a specific
	 * user's password as JSON encoded string. This is accessible to everyone, as
	 * this information is required to log a user in.
	 * 
	 * This method reads the following parameters from the request:
	 * <ul>
	 * <li><code>name</code>: The name of the user that wants to log in.</li>
	 * </ul>
	 * 
	 * @param request
	 *          the HTTP servlet request
	 * @param response
	 *          the HTTP servlet response
	 * @throws ServletException
	 *           if a database access error occurs
	 * @throws IOException
	 *           if an I/O problem occurs
	 */
	private void printUserCondiments(final HttpServletRequest request,
	    final HttpServletResponse response) throws ServletException, IOException {
		log.entry();
		String name = request.getParameter("name");
		JsonGenerator g = SaseSubmissionSystem.JSON_FACTORY
		    .createGenerator(response.getWriter());
		if (Utils.isStringEmpty(name)) {
			String msg = MessageFormat.format("Name {0} is null or empty.", name);
			throw new ServletException(msg);
		}
		try {
			DatabaseOperation op = new GetCondiments(g, name);
			exec.executeDbOperation(op);
		} finally {
			g.close();
		}
		log.exit();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * javax.servlet.http.HttpServlet#doGet(javax.servlet.http.HttpServletRequest,
	 * javax.servlet.http.HttpServletResponse)
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response)
	    throws ServletException, IOException {
		log.entry();
		AppResource r = mapServletPathToResource(request);
		switch (r) {
		case CONFIG_JSON:
			printAdminConfiguration(request, response);
			break;
		case CONDIMENTS_JSON:
			printUserCondiments(request, response);
			break;
		case USERS_JSON:
			printAdminUserList(request, response);
			break;
		default:
			throw new ServletException(generateUnmappedErrorMessage(request));
		}
		log.exit();
	}

}
