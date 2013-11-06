package at.iaik.teaching.sase.ku2013.protocolAnalysis;

import at.iaik.teaching.sase.ku2013.crypto.Fingerprint;

/**
 * Back-end database used by a revocation check service.
 */
public interface ResourceDatabase {
  /**
   * Queries the revocation check database for the status of a given
   * certificate.
   * 
   * This method performs a back-end specific query to the revocation database,
   * to determine if a given certificate is revoked, or not revoked.
   * Implementations should report {@link ResourceCheckResult#Revoked} if the
   * certificate is not known in the database. Note, that this behavior can not
   * be guaranteed, if an OCSP based back-end "database" is used. (See
   * {@link ResourceCheckResult#NotRevoked} for more details.)
   * 
   * 
   * @param cert
   *          The certificate to be checked for revocation.
   * 
   * @return {@link ResourceCheckResult#Revoked} if the certificate is known
   *         to be definitely revoked or unknown to ther server, 
   *         {@link ResourceCheckResult#NotRevoked} if the certificate is not 
   *         revoked.
   * 
   * @throws ResourceCheckException
   */
  ResourceCheckResult query(Fingerprint fp) throws ResourceCheckException;
}
