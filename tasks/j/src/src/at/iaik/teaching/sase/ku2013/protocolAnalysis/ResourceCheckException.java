package at.iaik.teaching.sase.ku2013.protocolAnalysis;

import java.io.IOException;
import java.security.GeneralSecurityException;

/**
 * Exception reported by the certificate validation subsystem.
 * 
 * This exception may be thrown by implementations of the
 * {@link CommunicationChannel}, {@link ResourceCheckServer} and
 * {@link ResourceChecker} interfaces to report unexpected errors during
 * processing of requests or responses.
 */
@SuppressWarnings("serial")
public final class ResourceCheckException extends Exception {
  /**
   * Constructs a new validation exception with just a message.
   * 
   * @param message
   *          The message text of this exception.
   */
  public ResourceCheckException(String message) {
    super(message);
  }

  /**
   * Constructs a new validation exception with a message and a cause.
   * 
   * This constructor is used to wrap checked exceptions (such as
   * {@link IOException}, {@link GeneralSecurityException}, etc) at the
   * {@link ResourceCheckServer} and {@link ResourceChecker} interface
   * boundaries.
   * 
   * 
   * @param message
   *          The message text of this exception.
   * 
   * @param cause
   *          The underlying exception, which causes this exception to be
   *          thrown. (may be null)
   */
  public ResourceCheckException(String message, Throwable cause) {
    super(message, cause);
  }
}
