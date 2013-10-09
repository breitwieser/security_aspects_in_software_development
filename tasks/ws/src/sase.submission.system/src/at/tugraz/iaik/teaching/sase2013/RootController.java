package at.tugraz.iaik.teaching.sase2013;

import static at.tugraz.iaik.teaching.sase2013.AppResource.SIGNIN;

import java.io.IOException;
import java.text.MessageFormat;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import at.tugraz.iaik.teaching.sase2013.db.CreateStudent;
import at.tugraz.iaik.teaching.sase2013.db.DatabaseOperation;
import at.tugraz.iaik.teaching.sase2013.db.DatabaseOperationExecutor;
import at.tugraz.iaik.teaching.sase2013.db.ExistsUserName;
import at.tugraz.iaik.teaching.sase2013.db.ResultDatabaseOperation;

/**
 * The controller for the root area of the SASE SUB SYSTEM. These are all the
 * functions that are available within the root realm. Currently, the following
 * functions are implemented:
 * <ul>
 * <li>User login error handling: If user login fails the controller dispatches
 * the user back to the login page and displays an error message.</li>
 * <li>User logout: logs out a user, by invalidating the user session.</li>
 * <li>Student sign-up: Creates a new student user within the SASE SUB SYSTEM
 * database.</li>
 * </ul>
 * 
 * @author SASE 2013 Team
 */
@WebServlet({ "/logout.do", "/signin.do", "/login-error.do" })
public class RootController extends GenericController {

	private static final long serialVersionUID = 1L;

	private static final DatabaseOperationExecutor exec = DatabaseOperationExecutor
	    .getInstance();

	private final static Logger log = LogManager
	    .getLogger(SaseSubmissionSystem.LOGGER_ID);

	/**
	 * Dispatches the user back to the login page and issues an error message
	 * informing the user that the login process failed.
	 * 
	 * @param request
	 *          the HTTP servlet request
	 * @param response
	 *          the HTTP servlet response
	 * @throws ServletException
	 *           if dispatching the user fails
	 * @throws IOException
	 *           if an I/O problem occurs
	 */
	protected void loginError(final HttpServletRequest request,
	    final HttpServletResponse response) throws ServletException, IOException {
		log.entry();
		Utils.dispatchCommunique("Zippy ate your login credentials and they gave "
		    + "him indigestion. We generally applaud this, but sadly we cannot "
		    + "log you in (Unknown user name and/or wrong password).", request,
		    response, AppResource.LOGIN);
		log.exit();
	}

	/**
	 * Logs a user out. Logging out a user is as simple as invalidating her
	 * session. As all user specific objects are stored in the user session this
	 * is sufficient to remove all user related resources.
	 * 
	 * @param request
	 *          the HTTP servlet request
	 * @param response
	 *          the HTTP servlet response
	 * @throws ServletException
	 *           if a problem occurs during log out or dispatch
	 * @throws IOException
	 *           if an I/O problem occurs during dispatching
	 */
	private void logout(HttpServletRequest request, HttpServletResponse response)
	    throws ServletException, IOException {
		request.getSession().removeAttribute(
		    SaseSubmissionSystem.SessionAttribute.USER_OBJECT_ID);
		request.getSession().invalidate();
		request.logout();
		RequestDispatcher rd = request.getRequestDispatcher("/index.jsp");
		rd.forward(request, response);
	}

	/**
	 * Tries to sign in (register) a new student with the SASE SUBMISSION SYSTEM.
	 * This method reads the following parameters from the request:
	 * <ul>
	 * <li><code>name</code>: The name of the new user.</li>
	 * <li><code>pass</code>: The password of the new user.</li>
	 * <li><code>email</code>: The email address of the new user.</li>
	 * <li><code>group</code>: The group number of the new user.</li>
	 * <li><code>iter</code>: The number of iterations the password has been
	 * digested.</li>
	 * <li><code>salt</code>: The salt used for password digestion.</li>
	 * </ul>
	 * 
	 * @param request
	 *          the HTTP servlet request
	 * @param response
	 *          the HTTP servlet response
	 * @throws ServletException
	 *           if a servlet specific problem occurs
	 * @throws IOException
	 *           if an I/O problem occurs
	 */
	protected void signIn(final HttpServletRequest request,
	    final HttpServletResponse response) throws ServletException, IOException {
		log.entry();
		String nameParam = request.getParameter("name");
		String passParam = request.getParameter("pass");
		String emailParam = request.getParameter("email");
		String groupParam = request.getParameter("group");
		String iterParam = request.getParameter("iter");
		String saltParam = request.getParameter("salt");

		/*
		 * Parameter validation
		 */
		if (Utils.isStringEmpty(nameParam) || Utils.isStringEmpty(passParam)
		    || Utils.isStringEmpty(emailParam) || Utils.isStringEmpty(groupParam)
		    || Utils.isStringEmpty(iterParam)) {
			Utils.dispatchCommunique(MessageFormat.format(
			    "At least one of the following is null: Name: {0}, Password {1}, "
			        + "Email: {2}, Group: {3}, #iterations {4}, salt {5}", nameParam,
			    passParam, emailParam, groupParam, iterParam, saltParam), request,
			    response, SIGNIN);
			return;
		}
		String name = nameParam.trim();
		if (!name.matches("\\w[\\p{Graph}\\s]*")) {
			Utils.dispatchCommunique(
			    MessageFormat.format("{0} is not a valid user name!", name), request,
			    response, SIGNIN);
			return;
		}
		String email = emailParam.trim();
		if (!email
		    .matches("(<)?\\w[\\p{Punct}\\w\\d-_%+ ]*@[\\w\\d-]+(\\.[\\w\\d-]+)*(>)?")) {
			Utils.dispatchCommunique(
			    MessageFormat.format("{0} is not a valid email address!", email),
			    request, response, SIGNIN);
			return;
		}
		String pass = passParam.trim();
		if (!testBigintegerParameter(pass, 16, "Password", request, response)) {
			return;
		}
		String salt = saltParam.trim();
		IntegerParameter iter = null, group = null;
		if (!((iter = testIntegerParameter(iterParam, "#Iterations", request,
		    response)).isValid())) {
			return;
		}
		if (!((group = testIntegerParameter(groupParam, "Group number", request,
		    response)).isValid())) {
			return;
		}
		/*
		 * End of parameter validation
		 * 
		 * TODO: TOCTOU Problem: User could be created between ExistsUser and
		 * CreateStudent. We could use a TRANSACTION with TRANSACTION_READ_COMMITTED
		 * isolation level to fix this, but his would complicate error handling ...
		 */
		ResultDatabaseOperation rop = new ExistsUserName(name);
		exec.executeDbOperation(rop);
		String user = (String) rop.getResult();
		if (!Utils.isStringEmpty(user)) {
			Utils.dispatchCommunique(
			    MessageFormat.format("The user name {0} is already in use.", user),
			    request, response, SIGNIN);
			return;
		}
		DatabaseOperation op = new CreateStudent(name, email, pass,
		    iter.getValue(), salt, group.getValue());
		exec.executeDbOperation(op);
		request.login(name, pass);
		RequestDispatcher rd = request
		    .getRequestDispatcher(AppResource.SUBMISSION_INDEX.getAppPath());
		rd.forward(request, response);
		log.exit();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * javax.servlet.http.HttpServlet#doGet(javax.servlet.http.HttpServletRequest,
	 * javax.servlet.http.HttpServletResponse)
	 */
	@Override
	protected void doGet(HttpServletRequest request, HttpServletResponse response)
	    throws ServletException, IOException {
		AppResource r = mapServletPathToResource(request);
		switch (r) {
		case LOGOUT:
			logout(request, response);
			break;
		default:
			throw new ServletException(generateUnmappedErrorMessage(request));
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * javax.servlet.http.HttpServlet#doPost(javax.servlet.http.HttpServletRequest
	 * , javax.servlet.http.HttpServletResponse)
	 */
	@Override
	protected void doPost(HttpServletRequest request, HttpServletResponse response)
	    throws ServletException, IOException {
		AppResource r = mapServletPathToResource(request);
		switch (r) {
		case SIGNIN_DO:
			signIn(request, response);
			break;
		case LOGIN_ERROR_DO:
			loginError(request, response);
			break;
		default:
			throw new ServletException(generateUnmappedErrorMessage(request));
		}
	}

}
