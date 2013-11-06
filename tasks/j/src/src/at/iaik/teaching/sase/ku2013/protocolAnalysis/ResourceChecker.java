package at.iaik.teaching.sase.ku2013.protocolAnalysis;

import iaik.x509.X509Certificate;

/**
 * Common interface for certificate revocation-check blackbox services.
 * 
 */
public interface ResourceChecker {
  /**
   * Checks a given certificate for its revocation status.
   * 
   * This method typically uses an underlying {@link CommunicationChannel} to
   * send a revocation check query to a {@link ResourceCheckServer}. Based on
   * the response of the server, the method returns the revocation status of the
   * certificate.
   * 
   * @param certificate
   *          The certificate to be checked.
   * 
   * @return The revocation check status reported by the server.
   * 
   * @throws ResourceCheckException
   *           may be thrown if processing of the certificate or communication
   *           with the server causes unexpected errors.
   */
  ResourceCheckResult check(X509Certificate certificate)
      throws ResourceCheckException;

}
