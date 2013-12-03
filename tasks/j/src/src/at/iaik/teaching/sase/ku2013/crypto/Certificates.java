package at.iaik.teaching.sase.ku2013.crypto;

import iaik.x509.X509Certificate;
import iaik.x509.extensions.BasicConstraints;
import iaik.x509.extensions.ExtendedKeyUsage;
import iaik.x509.extensions.KeyUsage;
import iaik.x509.ocsp.CertStatus;
import iaik.x509.ocsp.SingleResponse;

import java.io.File;
import java.io.FileFilter;
import java.io.IOException;
import java.security.InvalidKeyException;
import java.security.cert.CertificateException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import at.iaik.teaching.sase.ku2013.ocspmini.OcspMiniClient;

/**
 * Lookup table for searching certificates.
 */
public class Certificates {
  /**
   * Lookup table for finding certificates by their hash values (fingerprint).
   */
  private final Map<Fingerprint, X509Certificate> certs = new HashMap<Fingerprint, X509Certificate>();

  /**
   * Trusted root certificates
   */
  private final Set<Fingerprint> trusted_roots = new HashSet<Fingerprint>();

  /**
   * Constructs a new empty certificate table.
   */
  public Certificates() {

  }

  /**
   * Scans the given directory for certificate files with a extension
   * {@code .crt} and adds them to the certificate table.
   * 
   * This method scans the directory for regular files with {@code .crt}
   * extension and adds them to the certificate table. Note that this method
   * does not add anything to the table if parsing of the certificates fails.
   * 
   * @param directory
   *          Path of the directory to scan for certificates.
   * 
   * @return The number of newly added certificates. (This may be less than the
   *         number of certificate files in the directory in case that some
   *         certificates where already present in the table).
   * 
   * @throws IOException
   *           thrown in case of I/O problems.
   * 
   * @throws CertificateException
   *           thrown in case of certificate parsing problems.
   */
  public int addAllFromDirectory(String directory) throws IOException,
      CertificateException {
    // Resolve the directory
    File dir = new File(directory).getAbsoluteFile();
    if (!dir.isDirectory()) {
      throw new IOException("Given path is not a valid directory.");
    }

    // Find all files of interest
    File[] cert_files = dir.listFiles(new FileFilter() {
      @Override
      public boolean accept(File file) {
        return file.isFile() && file.canRead()
            && file.getName().endsWith(".crt");
      }
    });

    // Now load the certificates
    X509Certificate[] certs = new X509Certificate[cert_files.length];
    for (int n = 0; n < certs.length; ++n) {
      // And load the next certificate
      certs[n] = CryptoToolBox.loadCertificate(cert_files[n].getAbsolutePath());
    }

    // Finally add the certificates by their finger-prints. Duplicate
    // entries are automatically suppressed (same fingerprint = same
    // certificate).
    int new_certs = 0;

    for (int n = 0; n < certs.length; ++n) {
      if (add(certs[n])) {
        new_certs += 1;
      }
    }

    return new_certs;
  }

  /**
   * Loads an X.509 certificate from a disk file and adds it to the certificate
   * table.
   * 
   * @param filename
   *          File name of the certificate file to load.
   * 
   * @return Indicates if a new certificate was added (true) or if an existing
   *         entry was kept (false).
   * 
   * @throws CertificateException
   *           is thrown if the certificate is not parseable. Note that this
   *           exception does NOT provide any indication of the validity of the
   *           certificate.
   * 
   * @throws IOException
   *           is thrown if the the certificate could not be loaded due to an
   *           I/O error.
   */
  public boolean add(String filename) throws CertificateException, IOException {
    return add(CryptoToolBox.loadCertificate(filename));
  }

  /**
   * Adds a new certificate to the certificate table.
   * 
   * @param cert
   *          The certificate to be added.
   * 
   * @return Indicates if a new certificate was added (true) or if an existing
   *         entry was kept (false).
   */
  public boolean add(X509Certificate cert) {
    if (cert == null) {
      throw new IllegalArgumentException("Certificate must not be null.");
    }

    // Add into the finger-print table (if not already contained)
    Fingerprint fingerprint = new Fingerprint(cert);
    if (this.certs.containsKey(fingerprint)) {
      return false;
    }

    this.certs.put(fingerprint, cert);
    return true;
  }

  /**
   * Searches a certificate by its fingerprint.
   * 
   * This method does not provide any kind of certificate chain checking. It can
   * be used to resolve any valid or invalid certificates.
   * 
   * @param fingerprint
   *          The fingerprint of the certificate to search.
   * 
   * @param intended_usage
   *          Intended usage of the public key in the certificate or null to
   *          skip the usage check.
   * 
   * @return The requested certificate or null if no suitable certificate could
   *         be found. (null is only returned when no intended usage is given.)
   * 
   * @throws CertificateException
   *           may be thrown by the validity check, for example if the
   *           certificate has been corrupted of if the intended key usage
   *           indicated by the {@code intended_usage} parameter does not match
   *           the constraints of the certificate.
   */
  public X509Certificate use(Fingerprint fingerprint,
      IntendedUsage intended_usage) throws CertificateException {

    X509Certificate cert = this.certs.get(fingerprint);
    if (intended_usage != null) {
      checkUsage(cert, intended_usage);
    }

    return cert;
  }

  /**
   * Gets the ordered certificate chain for a certificate with the given
   * finger-print.
   * 
   * This method fails if more than one path to a trusted root certificate exist
   * (which may be the case for cross-certification).
   * 
   * @param leaf
   *          The leaf certificate of the certificate chain.
   * 
   * @return The requested certificate chain.
   * 
   * @throws CertificateException
   *           if the chain can not be build.
   */
  public X509Certificate[] getUniqueChain(X509Certificate leaf)
      throws CertificateException {
    if (leaf == null) {
      throw new CertificateException(
          "Leaf certificate does not exist in this repository.");
    }

    return CryptoToolBox.orderCertificateChain(leaf, toCertificateArray());
  }

  /**
   * Gets the ordered certificate chain for a certificate with the given
   * fingerprint.
   * 
   * @param fingerprint
   *          Fingerprint of the leaf-certificate in the chain.
   * 
   * @return The requested certificate chain.
   * 
   * @throws CertificateException
   *           if the chain can not be build.
   */
  public X509Certificate[] getUniqueChain(Fingerprint fingerprint)
      throws CertificateException {
    return getUniqueChain(use(fingerprint, null));
  }

  /**
   * Gets all certificate chains, which start at the given certificate and end
   * at root certificate.
   * 
   * @param leaf
   *          The leaf certificate
   * @return
   * 
   * @throws CertificateException
   *           thrown if chain construction is not possible.
   */
  public List<X509Certificate[]> getChains(X509Certificate leaf)
      throws CertificateException {

    // TODO: This method currently fails if more than one certificate
    // chain from the leaf certificate to a self-signed root certificate
    // exist. Extend this method to find all certificate chains starting
    // at the leaf certificate and ending at a self-signed root certificate.
    //
    // HINT: Use X509Certificate#getSubjectDN and
    // X509Certificate#getIssuerDN
    // to correlate the certificates. It may be advisable to first implement
    // the missing parts of checkUsageInChain below before working on cross
    // certification.

    ArrayList<X509Certificate[]> chains = new ArrayList<X509Certificate[]>();
    // ----- BEGIN STUDENT CODE -----
    // NOTE: Dummy implementation to allow use with simple chains without cross-certification ...
    
//    System.out.println(cha)
    //algorithm
    //start with leaf
    //get issuer id
    
    
    
    System.out.println("############################################");
    System.out.println("############################################");
    System.out.println("leaf: " + leaf);
    System.out.println(leaf.getIssuerDN());
    
    System.out.println("############################################");
    
    chains.add(getUniqueChain(leaf));
    
    X509Certificate[] chain = chains.get(0);
    for(X509Certificate cert: chain)
    {
    	System.out.println("subject: " + cert.getSubjectDN());
    	System.out.println("issuer: " + cert.getIssuerDN());
    	System.out.println("");
    }
    
    System.out.println("############################################");

    // ----- END STUDENT CODE -----

    return chains;
  }

  /**
   * Checks the validity of a certificate including its chain and verifies that
   * a certificate is usable for a certain purpose.
   * 
   * @param leaf
   *          The leaf certificate to be checked.
   * 
   * @param intended_usage
   *          Intended usage of the certificate.
   * 
   * @throws CertificateException
   *           thrown if the certificate is invalid, corrupted, expired or
   *           incompatible with the intended usage.
   */
  protected void checkUsage(X509Certificate leaf, IntendedUsage intended_usage)
      throws CertificateException {

    // This method is already ready for cross-certification support. To
    // fully
    // implement cross-certification, simply finish the getChains method.

    List<X509Certificate[]> chains = getChains(leaf);

    // Sanity check: Did we miss something?
    if (chains.size() == 0) {
      throw new CertificateException(
          "Unexpected chain construction error (no chains found)");
    }

    // Now validate for each chain
    for (X509Certificate[] chain : chains) {
      // Sanity check: Leaf certificate must be at index 0
      if (chain[0] != leaf) {
        throw new CertificateException(
            "Unexpected chain construction error (leaf is not at index 0)");
      }

      // And delegate to our chain verifier
      checkUsageInChain(chain, intended_usage);
    }
  }

  /**
   * Checks if the given certificate has been revoked.
   * 
   * @throws IOException
   * @throws InvalidKeyException
   * 
   */

  protected boolean isRevoked(X509Certificate cert) throws InvalidKeyException {
    /*
     * This is only a demo, how a simple OCSP request could be performed. You do
     * not have to touch the *.ocspmini.* packages for this pracical exercise!
     */
    SingleResponse status_response = OcspMiniClient.performRequest(cert);

    if (!(status_response.getCertStatus().getCertStatus() == CertStatus.GOOD)) {
      System.err.println("OCSP server returned, that certificate was revoked");
      return true;
    }
    return false;
  }

  /**
   * Checks the validity of a certificate including its chain and verifies that
   * a certificate is usable for a certain purpose.
   * 
   * @param chain
   *          Ordered certificate to be validated. The leaf certificate to be
   *          used is at index 0, the root CA certificate is at the last index.
   * 
   * @param intended_usage
   *          Intended usage of the certificate.
   * 
   * @throws CertificateException
   *           thrown if the certificate is invalid, corrupted, expired or
   *           incompatible with the intended usage.
   */
  protected void checkUsageInChain(X509Certificate[] chain,
      IntendedUsage intended_usage) throws CertificateException {

    // TODO: Implement certificate chain validation and usage checks.

	  X509Certificate root = chain[chain.length-1];
	  // 1) root is trusted
	  if(!isTrusted(root))
		  throw new CertificateException("Certificate Root untrusted");
	  for(int i = 0; i < chain.length; i++)
	  {
		  try
		  {
			  // 2) each has a valid signature
			  if(i < (chain.length-1))
				  chain[i].verify(chain[i+1].getPublicKey());
			  // 3) each valid at current time
			  chain[i].checkValidity();
			  // 4) BasicConstraints present and marked critical
			  //* CA: must be PRESENT and CRITICAL
			  //* LEAF: must be PRESENT (may be critical)
			  if(chain[i].getExtension(BasicConstraints.oid) == null)
				  throw new CertificateException("BasicConstraints not present");
			  BasicConstraints basic = (BasicConstraints)chain[i].getExtension(BasicConstraints.oid);
			  if(i != 0 && !basic.ca())
				  throw new CertificateException("Non CA in chain detected");
				  
			  if(basic.ca())
			  {
				  if(!basic.isCritical())
					  throw new CertificateException("CA, but BasicConstraints not marked as critical");
				  // 5) BasicConstraints extension properly reflects the usage as CA or non-CA certificate.
				  // ??
				  // 6) Check the path length constraints found in BasicConstraints for CA certificates.
				  int pathlenconstraint = basic.getPathLenConstraint();
				  if(pathlenconstraint != -1)
				  {
					  if(pathlenconstraint < (i-1))
					  	throw new CertificateException("Path length constraints violated");
				  }
			  }
			  // 7) KeyUsage extension is present and matches the intended key usage. 
			  // See the TODOs in the  IntendedUsage enumeration class for more details.
			  if(chain[i].getExtension(KeyUsage.oid) == null)
				  throw new CertificateException("KeyUsage not present");
			  KeyUsage keyusage = (KeyUsage)chain[i].getExtension(KeyUsage.oid);
			  if(!intended_usage.isAllowedBy(basic, keyusage))
				  throw new CertificateException("KeyUsage doesn't match intended key usage");
			  // 8) No unhandled critical certificate extensions are present in any of the certificates. 
			  // The only allowed critical extensions are BasicConstraints, KeyUsage and ExtendedKeyUsage. 
			  // The ExtendedKeyUsage extension may be missing.
			  if(chain[i].hasUnsupportedCriticalExtension())
				  throw new CertificateException("Unsupported Critical Extension found");
			  Set<?> criticalexts = chain[i].getCriticalExtensionOIDs();
			  criticalexts.remove(BasicConstraints.oid.getID());
			  if(!criticalexts.remove(KeyUsage.oid.getID()))
				  System.out.println("KeyUsage not Critical");
			  // TODO: should throw exception??
			//	  throw new CertificateException("KeyUsage not Critical");
			  criticalexts.remove(ExtendedKeyUsage.oid.getID());
			  if(!criticalexts.isEmpty())
				  throw new CertificateException("To much Critical extensions");
			  // 9) No certificate in the chain has been revoked. 
			  // Use the Certificates.isRevoked method to invoke the OCSPMiniClient for performing this check.
			  if(isRevoked(chain[i]))
				  throw new CertificateException("Certificate revoked");
		  } catch(Exception e)
		  {
			  //e.printStackTrace();
			  throw new CertificateException(e.getMessage());
		  }
	  }
  }

  /**
   * Returns an new array containing all certificates in this table.
   * 
   * @return An array containing all certificates in this table.
   */
  public X509Certificate[] toCertificateArray() {
    X509Certificate[] certs = new X509Certificate[this.certs.size()];
    return this.certs.values().toArray(certs);
  }

  /**
   * Adds the finger-print of a trusted root certificate.
   * 
   * @param fingerprint
   *          The fingerprint of the certificate to be added. Note that the
   *          referenced certificate may be added to the table at a later time.
   * 
   * @return Indicates if a new fingerprint was added or if the fingerprint
   *         already existed.
   */
  public boolean addTrusted(Fingerprint fingerprint) {
    if (fingerprint == null) {
      throw new IllegalArgumentException("Fingerprint must not be null.");
    }

    return this.trusted_roots.add(fingerprint);
  }

  /**
   * Adds a trusted root certificate.
   * 
   * This method first adds the certificate to the table and then registers its
   * fingerprint as trusted root fingerprint.
   */
  public void addTrusted(X509Certificate cert) {
    Fingerprint fingerprint = new Fingerprint(cert);
    add(cert);
    addTrusted(fingerprint);
  }

  /**
   * Tests if a given certificate is a trusted certificate.
   * 
   * @param fingerprint
   *          The fingerprint to test.
   * 
   * @return True if the fingerprint is trusted, false otherwise.
   */
  public boolean isTrusted(Fingerprint fingerprint) {
    return this.trusted_roots.contains(fingerprint);
  }

  /**
   * Tests if a given certificate is a trusted certificate.
   * 
   * @param cert
   *          The certificate to test.
   * 
   * @return True if the fingerprint is trusted, false otherwise.
   */
  public boolean isTrusted(X509Certificate cert) {
    return isTrusted(new Fingerprint(cert));
  }

  /**
   * Returns an array containing all trusted root certificate fingerprints.
   * 
   * @return An array with the root fingerprints.
   */
  public Fingerprint[] getTrusted() {
    Fingerprint[] ids = new Fingerprint[this.trusted_roots.size()];
    return this.trusted_roots.toArray(ids);
  }
}
