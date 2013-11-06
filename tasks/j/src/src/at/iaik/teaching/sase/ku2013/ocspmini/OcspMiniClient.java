package at.iaik.teaching.sase.ku2013.ocspmini;

import iaik.asn1.CodingException;
import iaik.asn1.structures.AlgorithmID;
import iaik.asn1.structures.GeneralName;
import iaik.asn1.structures.Name;
import iaik.security.provider.IAIK;
import iaik.x509.X509Certificate;
import iaik.x509.X509ExtensionException;
import iaik.x509.ocsp.BasicOCSPResponse;
import iaik.x509.ocsp.OCSPException;
import iaik.x509.ocsp.OCSPRequest;
import iaik.x509.ocsp.OCSPResponse;
import iaik.x509.ocsp.ReqCert;
import iaik.x509.ocsp.Request;
import iaik.x509.ocsp.SingleResponse;
import iaik.x509.ocsp.extensions.Nonce;

import java.io.IOException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PrivateKey;
import java.security.SecureRandom;
import java.security.SignatureException;
import java.security.cert.CertificateException;

import at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox;
import at.iaik.teaching.sase.ku2013.crypto.Fingerprint;

public class OcspMiniClient {

  /**
   * The private key for the client certificate.
   */
  private static final String OCSP_CLIENT_KEY = "examples/validation/Client Key.pem";

  /**
   * The password for the private key.
   */
  private static final String OCSP_CLIENT_PWD = "sase";

  /**
   * Validation certificate
   */
  private X509Certificate client_cert_;

  /**
   * Private key to sign the request with.
   */
  private PrivateKey client_key_;

  /**
   * Request the IAIK provider.
   */
  static {
    IAIK.addAsProvider();
  }

  /**
   * Instantiate the OCSP mini client.
   */
  public OcspMiniClient(PrivateKey client_key, X509Certificate client_cert) {
    this.client_key_ = client_key;
    this.client_cert_ = client_cert;
  }

  /**
   * Build an an OCSP request.
   * 
   * @param certs
   *          Certificates to be checked in this request.
   * 
   * @throws NoSuchAlgorithmException
   * @throws X509ExtensionException
   * @throws OCSPException
   * @throws InvalidKeyException
   * @throws CodingException
   */
  private OCSPRequest buildOcspRequest(X509Certificate cert)
      throws NoSuchAlgorithmException, X509ExtensionException,
      InvalidKeyException, OCSPException, CodingException {

    /* Construct the request */
    OCSPRequest req = new OCSPRequest();

    /* Construct the request list for the OCSP request */
    Request[] request_list = new Request[1];
    X509Certificate target_cert = cert;
    ReqCert req_cert = new ReqCert(ReqCert.pKCert, target_cert);
    request_list[0] = new Request(req_cert);

    /* OCSP request nonce */
    SecureRandom rng = new SecureRandom();
    byte[] ocsp_nonce_value = new byte[32];
    rng.nextBytes(ocsp_nonce_value);

    /* Setup the certificate request */
    req.setRequestList(request_list);
    req.addExtension(new Nonce(ocsp_nonce_value));

    Name subject_dn = (Name) this.client_cert_.getSubjectDN();
    req.setRequestorName(new GeneralName(GeneralName.directoryName, subject_dn));

    X509Certificate[] chain = new X509Certificate[1];
    chain[0] = cert;

    /* Set the signer certificates and sign the OCSP request */
    req.setCertificates(chain);
    req.sign(AlgorithmID.sha1WithRSAEncryption, this.client_key_);

    return req;
  }

  /**
   * Send an OCSP request.
   * 
   * @param certs
   *          certificate chain, with the certificate to be checked as leaf.
   * 
   * @return singleRespnse the singleResponse containing the status of the
   *         certificate.
   * 
   * @throws CodingException
   * @throws OCSPException
   * @throws X509ExtensionException
   * @throws NoSuchAlgorithmException
   * @throws InvalidKeyException
   * @throws IOException
   * @throws CertificateException
   * @throws SignatureException
   * @throws NoSuchProviderException
   */
  private SingleResponse sendOcspRequest(X509Certificate cert)
      throws InvalidKeyException, NoSuchAlgorithmException,
      X509ExtensionException, OCSPException, CodingException, IOException,
      NoSuchProviderException, SignatureException, CertificateException {

    OcspMiniServer.setup();
    
    /* Build the request */
    OCSPRequest req = buildOcspRequest(cert);

    /* Send request to the OCSP server */
    OCSPResponse resp = OcspMiniServer.processRequest(req);

    /* Extract the basic response from the OCSPResponse */
    BasicOCSPResponse basicOcspResp = (BasicOCSPResponse) resp.getResponse();

    /* Get the requested certificate */
    ReqCert req_cert = new ReqCert(ReqCert.pKCert, cert);

    /* Get the response for the certificate */
    SingleResponse singleResponse = basicOcspResp.getSingleResponse(req_cert);

    System.err.println(new Fingerprint(cert) + " has OCSP status: "
        + singleResponse.getCertStatus());

    return singleResponse;
  }

  /**
   * Main entry point for the demo client.
   * 
   * @throws IOException
   * @throws InvalidKeyException
   */
  public static SingleResponse performRequest(X509Certificate client_cert)
      throws InvalidKeyException {

    SingleResponse resp = null;
    PrivateKey client_key = null;

    try {
      client_key = CryptoToolBox.loadPemPrivateKey(OCSP_CLIENT_KEY,
          OCSP_CLIENT_PWD.toCharArray());
    } catch (Exception e) {
      System.err.println("! error loading the OCSP Client Private Key!");
      e.printStackTrace();
      System.exit(1);
    }

    try {
      OcspMiniClient client = new OcspMiniClient(client_key, client_cert);
      resp = client.sendOcspRequest(client_cert);

    } catch (Exception e) {
      System.err.println("! client side exception");
      e.printStackTrace();
      System.exit(2);
    }
    return resp;
  }
}
