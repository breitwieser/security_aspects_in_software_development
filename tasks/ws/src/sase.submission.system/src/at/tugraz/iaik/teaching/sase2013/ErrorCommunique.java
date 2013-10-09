/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.Serializable;
import java.io.Writer;

import javax.servlet.http.HttpSession;

import com.fasterxml.jackson.core.JsonGenerator;

import static at.tugraz.iaik.teaching.sase2013.SaseSubmissionSystem.SessionAttribute;

/**
 * Contains everything to communicate a non fatal error to the user. Non fatal
 * meaning that it is something the user can fix, for example by specifying
 * different parameters to a given request. Currently, supports only one
 * communique at the time.
 * 
 * @author SASE 2013 Team
 */
public class ErrorCommunique implements Serializable {

	/**
	 * Auto generated serial version uid.
	 */
  private static final long serialVersionUID = -7086312983351313546L;
	private final String message;
	private final Throwable t;

	/**
	 * Creates a new <code>ErrorCommunique</code> with the given message.
	 * 
	 * @param message
	 *          the message to present to the user
	 */
	public ErrorCommunique(String message) {
		this(message, null);
	}

	/**
	 * Creates a new <code>ErrorCommunique</code> with the given message and
	 * throwable. The throwable should not be communicated to the user, because
	 * such practice is confusing at best, or helpful to an attacker at worst.
	 * Still the information is useful for debugging.
	 * 
	 * @param message
	 *          the message to present to the user
	 * @param t
	 *          the throwable containing further error specifics
	 */
	public ErrorCommunique(String message, Throwable t) {
		super();
		this.message = message;
		this.t = t;
	}

	/**
	 * @return the error message to present to the user
	 */
	public String getMessage() {
		return message;
	}

	/**
	 * @return the throwable containing further error specifics, if set
	 */
	public Throwable getThrowable() {
		return t;
	}

	/**
	 * @param s
	 *          the HttpSession to register the error communique with
	 */
	public void registerCommunique(HttpSession s) {
		s.setAttribute(SessionAttribute.ERROR_COMMUNIQUE, this);
	}

	/**
	 * @param s
	 *          the HttpSession to get the error communique from
	 * @return the error communique if one exists; null otherwise
	 */
	public static ErrorCommunique getCommunique(HttpSession s) {
		return (ErrorCommunique) s.getAttribute(SessionAttribute.ERROR_COMMUNIQUE);
	}

	/**
	 * @param s
	 *          the HttpSession to clear any error communique from
	 */
	public static void clearCommunique(HttpSession s) {
		s.removeAttribute(SessionAttribute.ERROR_COMMUNIQUE);
	}

	/**
	 * Generates a JSON object representing the error communique. The JSON object
	 * contains a message to the client, as well as the base64 encoded stack trace
	 * of the associated exception.
	 * 
	 * @param out
	 *          the output writer to write the generated JSON to
	 * @throws IOException
	 *           if an I/O error occurs
	 */
	public void toJson(Writer out) throws IOException {
		JsonGenerator g = SaseSubmissionSystem.JSON_FACTORY.createGenerator(out);
		try {
			g.writeStartObject();
			g.writeStringField("Message", message);
			ByteArrayOutputStream bout = new ByteArrayOutputStream();
			PrintWriter tout = new PrintWriter(bout);
			Throwable t = this.t;
			g.writeFieldName("Throwables");
			g.writeStartArray();
			while (t != null) {
				tout.println(t.getMessage());
				t.printStackTrace(tout);
				tout.flush();
				g.writeBinary(bout.toByteArray());
				bout.reset();
				t = t.getCause();
			}
			g.writeEndArray();
			g.writeEndObject();
		} finally {
			g.close();
		}
	}

}
