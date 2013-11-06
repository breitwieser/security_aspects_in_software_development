package at.iaik.teaching.sase.ku2013.protocolAnalysis;

/**
 * Certificate revocation-check results.
 */
public enum ResourceCheckResult {
  /**
   * According to the revocation-check service, this certificate has not been
   * revoked.
   * 
   * This status indicates that, that a certificate has not been revoked
   * according to a revocation-check service. OCSP based services may report
   * this status for unknown certificates (based on the reasoning, that no
   * statement can be made about the revocation status.)
   * 
   * User application is still responsible for checking the certificate chains,
   * signatures, validity periods, extensions, etc.
   */
  NotRevoked,

  /**
   * The certificate has been revoked according to the revocation-check service.
   * 
   * This status indicates, that a certificate is definitely revoked according
   * to the revocation-check service.
   */
  Revoked
}
