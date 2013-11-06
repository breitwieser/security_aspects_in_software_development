package at.iaik.teaching.sase.ku2013.ocspmini;

import iaik.asn1.structures.AlgorithmID;
import iaik.security.provider.IAIK;
import iaik.utils.SSLeayPrivateKey;
import iaik.x509.X509Certificate;
import iaik.x509.extensions.ReasonCode;
import iaik.x509.ocsp.BasicOCSPResponse;
import iaik.x509.ocsp.CertStatus;
import iaik.x509.ocsp.OCSPException;
import iaik.x509.ocsp.OCSPRequest;
import iaik.x509.ocsp.OCSPResponse;
import iaik.x509.ocsp.ReqCert;
import iaik.x509.ocsp.ResponderID;
import iaik.x509.ocsp.RevokedInfo;
import iaik.x509.ocsp.SingleResponse;

import java.io.FileInputStream;
import java.io.IOException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PrivateKey;
import java.security.SignatureException;
import java.security.cert.CertificateException;
import java.util.Arrays;
import java.util.Date;
import java.util.List;

import at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox;
import at.iaik.teaching.sase.ku2013.crypto.Fingerprint;

/**
 * Minimalistic OCSP server (responder) demo.
 * 
 * @author Johannes Winter <johannes.winter@iaik.tugraz.at>
 * 
 */
public class OcspMiniServer {

  /**
   * The OCSP server certificate.
   */
  private static final String OCSP_SERVER_CERT = "examples/validation/Server.crt";
  
  /**
   * The private key used for signing the OCSP response.
   */
  private static final String OCSP_SERVER_KEY = "examples/validation/Server Key.pem";
  
  /**
   * The password for the private key.
   */
  private static final String OCSP_PWD = "sase";
  
  /**
   * Server's signature key.
   */
  private static PrivateKey ocsp_key = null;

  /**
   * Server's certificate.
   */
  private static X509Certificate ocsp_cert = null;
  
  private static List<String> blacklist = null;

  /**
   * Install the IAIK-JCE provider.
   */
  static {
    IAIK.addAsProvider();
  }
  
  public static List<String> setUpBlacklist() {
    
    List<String> list = Arrays.asList(
        "15:0E:79:25:96:5B:66:50:99:25:38:7B:FE:B6:F1:33:1C:11:64:FB");
    return list;
    
  }

  /**
   * Create a simple CA revoked response object.
   * 
   * @return
   */
  public static SingleResponse createRevokedResponse(X509Certificate cert,
      Date revocation_date, int reason_code) {

    ReqCert req_cer = new ReqCert(ReqCert.pKCert, cert);
    RevokedInfo revoke_info = new RevokedInfo(revocation_date);
    revoke_info.setRevocationReason(new ReasonCode(reason_code));

    CertStatus cert_status = new CertStatus(revoke_info);
    return new SingleResponse(req_cer, cert_status, new Date());
  }

  /**
   * Create a simple certificate valid response.
   * 
   * @param cert
   * @return
   */
  public static SingleResponse createValidResponse(X509Certificate cert) {
    ReqCert req_cer = new ReqCert(ReqCert.pKCert, cert);
    return new SingleResponse(req_cer, new CertStatus(), new Date());
  }

  /**
   * Run the OCSP mini server.
   * 
   * @param args
   * @throws IOException
   * @throws CertificateException
   * @throws InvalidKeyException
   * @throws NoSuchAlgorithmException
   */
  public static void setup() throws IOException, InvalidKeyException,
      CertificateException {
    
    blacklist = setUpBlacklist();

    ocsp_cert = CryptoToolBox.loadCertificate(OCSP_SERVER_CERT);

    FileInputStream is = new FileInputStream(OCSP_SERVER_KEY);

    SSLeayPrivateKey encoded_key = new SSLeayPrivateKey(is);

    encoded_key.decrypt(OCSP_PWD);

    ocsp_key = encoded_key.getPrivateKey();

    is.close();
  }

  /**
   * @param req
   *          the request containing the certificate to be checked.
   * @return the OCSP response.
   * 
   * @throws InvalidKeyException
   * @throws NoSuchAlgorithmException
   * @throws NoSuchProviderException
   * @throws SignatureException
   * @throws OCSPException
   */
  public static OCSPResponse processRequest(OCSPRequest req)
      throws InvalidKeyException, NoSuchAlgorithmException,
      NoSuchProviderException, SignatureException, OCSPException {

    SingleResponse resp = null;
    
    X509Certificate certificate = req.getCertifcates()[0];
    Fingerprint fp = new Fingerprint(certificate);
    
    if(blacklist.contains(fp.toString())) {
      resp = createRevokedResponse(certificate, new Date(),
          ReasonCode.aACompromise);
    }
    else
      resp = createValidResponse(certificate);

    // Build the response.
    BasicOCSPResponse bOcspResp = new BasicOCSPResponse();
    ResponderID respId = new ResponderID(ocsp_cert.getPublicKey());

    bOcspResp.setResponderID(respId);
    bOcspResp.setProducedAt(new Date());
    bOcspResp.setSingleResponses(new SingleResponse[] { resp });

    X509Certificate[] chain = new X509Certificate[1];

    chain[0] = ocsp_cert;

    bOcspResp.setCertificates(chain);
    bOcspResp.sign(AlgorithmID.sha1WithRSAEncryption, ocsp_key);

    OCSPResponse ocspResponse = new OCSPResponse(bOcspResp);

    return ocspResponse;
  }
}
