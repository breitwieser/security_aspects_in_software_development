/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013.db;

/**
 * Signals a logical problem during a database operation. In most cases this is
 * an indication of a database consistency problem.
 * 
 * @author SASE 2013 Team
 */
public class DatabaseOperationException extends Exception {

	/**
	 * Auto generated serial version uid.
	 */
  private static final long serialVersionUID = 4628815635289943082L;

	/**
	 * Creates a new <code>DatabaseOperationException</code>.
	 */
	public DatabaseOperationException() {
		super();
	}

	/**
	 * Creates a new <code>DatabaseOperationException</code> with the given error
	 * message.
	 * @param message the error message
	 */
	public DatabaseOperationException(String message) {
		super(message);
	}

	/**
	 * Creates a new <code>DatabaseOperationException</code> with the given cause.
	 * @param cause the cause
	 */
	public DatabaseOperationException(Throwable cause) {
		super(cause);
	}

	/**
	 * Creates a new <code>DatabaseOperationException</code> with the given error
	 * message and cause.
	 * @param message the error message
	 * @param cause the cause
	 */
	public DatabaseOperationException(String message, Throwable cause) {
		super(message, cause);
	}

	/**
	 * Creates a new exception with the given detail message, cause, suppression
	 * enabled or disabled, and writable stack trace enabled or disabled.
	 * 
	 * @param message
	 *          the message
	 * @param cause
	 *          the cause
	 * @param enableSuppression
	 *          true to enable suppression; false to disable
	 * @param writableStackTrace
	 *          true to enable writeable stack trace; false to disable
	 */
	public DatabaseOperationException(String message, Throwable cause,
	    boolean enableSuppression, boolean writableStackTrace) {
		super(message, cause, enableSuppression, writableStackTrace);
	}

}
