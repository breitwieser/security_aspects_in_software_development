/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013;

import static at.tugraz.iaik.teaching.sase2013.AppResource.SIGNIN;

import java.io.IOException;
import java.math.BigInteger;
import java.text.MessageFormat;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * Base class for controllers providing methods to facilitate dispatching
 * specific requests to their implementing methods, and parsing and type
 * checking of request parameters.
 * 
 * @author SASE 2013 Team
 */
public class GenericController extends HttpServlet {

	/** Auto-generated serial version unique identifier. */
	private static final long serialVersionUID = 173883517253348769L;

	/**
	 * A simple container class for an integer value parsed from a string. The
	 * <code>isIntParam()</code> can be used to determine if parsing the string
	 * succeeded, or failed, and thus if the contained value is a valid integer
	 * parameter.
	 * 
	 * @author SASE 2013 Team
	 */
	protected class IntegerParameter {

		private final int value;
		private final boolean intParam;

		/**
		 * Creates a new <code>IntegerParameter</code> for an invalid parameter,
		 * that is a parameter that could not be parsed.
		 */
		public IntegerParameter() {
			value = 0;
			intParam = false;
		}

		/**
		 * Creates an new <code>IntegerParameter</code> with the given valid integer
		 * value.
		 * 
		 * @param value
		 *          the valid integer parameter value
		 */
		public IntegerParameter(int value) {
			super();
			this.value = value;
			this.intParam = true;
		}

		/**
		 * @return the integer parameter, if it is valid, otherwise a
		 *         NumberFormatException is thrown
		 * @throws NumberFormatException
		 *           if the integer parameter is inavlid
		 */
		public int getValue() throws NumberFormatException {
			if (!intParam)
				throw new NumberFormatException();
			return value;
		}

		/**
		 * @return true if the integer parameter is valid; false otherwise
		 */
		public boolean isValid() {
			return intParam;
		}

	}

	/**
	 * Tries to parse the given parameter as an integer. If parsing fails, an
	 * error message is registered with the user's session.
	 * 
	 * @param param
	 *          the parameter to test
	 * @param name
	 *          the name of the parameter that is used by the error message
	 * @param request
	 *          the HTTP servlet request
	 * @param response
	 *          the HTTP servlet response
	 * @return an integer parameter telling the caller if parsing succeeded and if
	 *         so also contains the value
	 * @throws ServletException
	 *           if dispatching the error message fails
	 * @throws IOException
	 *           if dispatching the error message fails due to an I/O problem
	 */
	protected IntegerParameter testIntegerParameter(String param, String name,
	    HttpServletRequest request, HttpServletResponse response)
	    throws ServletException, IOException {
		try {
			int value = Integer.parseInt(param);
			return new IntegerParameter(value);
		} catch (NumberFormatException e) {
			Utils.dispatchCommunique(
			    MessageFormat.format("{0} {1} is not a number!", param), e, request,
			    response, SIGNIN);
			return new IntegerParameter();
		}
	}

	/**
	 * Tries to parse the given parameter as a {@link BigInteger} with the given
	 * radix. If parsing fails, an error message is registered with the user's
	 * session.
	 * 
	 * @param param
	 *          the parameter to test
	 * @param radix
	 *          the radix of the number system to use for parsing
	 * @param name
	 *          the name of the parameter that is used by the error message
	 * @param request
	 *          the HTTP servlet request
	 * @param response
	 *          the HTTP servlet response
	 * @return true if parameter is a valid BigInteger; false otherwise
	 * @throws ServletException
	 *           if dispatching the error message fails
	 * @throws IOException
	 *           if dispatching the error message fails due to an I/O problem
	 */
	protected boolean testBigintegerParameter(String param, int radix,
	    String name, HttpServletRequest request, HttpServletResponse response)
	    throws ServletException, IOException {
		try {
			new BigInteger(param, radix);
			return true;
		} catch (NumberFormatException e) {
			Utils.dispatchCommunique(
			    MessageFormat.format("{0} {1} is not a number!", param), e, request,
			    response, SIGNIN);
		}
		return false;
	}

	/**
	 * Generates the invalid servlet mapping error message.
	 * 
	 * @param request
	 *          the <code>HttpServletRequest</code> to this servlet
	 * @return the error message
	 */
	protected String generateUnmappedErrorMessage(HttpServletRequest request) {
		return MessageFormat.format(
		    "Servlet {0} is mapped to {1} but does not support this operation.",
		    getClass().getName(), request.getServletPath());
	}

	/**
	 * Tries to map the servlet path of the given <code>HttpServletRequest</code>
	 * to an <code>AppResource</code>. If no <code>AppResource</code> is
	 * registered for the given servlet path, this method throws a
	 * <code>ServletException</code>.
	 * 
	 * @param request
	 *          the <code>HttpServletRequest</code> to this servlet, which
	 *          contains the servlet path to map to an <code>AppResource</code>
	 * @param errMsg
	 *          the error message if the mapping process fails
	 * @return the <code>AppResource</code> for the given servlet path
	 * @throws ServletException
	 *           if no <code>AppResource</code> is mapped to the given path
	 */
	protected AppResource mapServletPathToResource(HttpServletRequest request)
	    throws ServletException {
		final String path = request.getServletPath();
		AppResource r = AppResource.getResourceFromServletPath(path);
		if (r == null) {
			throw new ServletException(generateUnmappedErrorMessage(request));
		}
		return r;
	}

}
