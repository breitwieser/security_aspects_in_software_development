package at.iaik.teaching.sase.ku2013.server;

import iaik.security.provider.IAIK;
import iaik.x509.X509Certificate;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PrivateKey;
import java.security.SignatureException;
import java.security.cert.CertificateException;
import java.util.Properties;

import javax.crypto.BadPaddingException;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.SecretKey;

import at.iaik.teaching.sase.ku2013.codlet.Codlet;
import at.iaik.teaching.sase.ku2013.crypto.Certificates;
import at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox;
import at.iaik.teaching.sase.ku2013.crypto.Fingerprint;
import at.iaik.teaching.sase.ku2013.crypto.IntendedUsage;

/**
 * Server receives a signed byte array from the client and decrypts it using
 * KEM/DEM mechanism. The the server tries to verify the signature and sends it
 * to an OCSP server to check the certificate's revocation status.
 * 
 * @author SASE 2013 Team
 * 
 */
public final class Server {

  /**
   * Received encrypted data.
   */
  private byte[] clientData = null;

  /**
   * Initial value for AES decryption.
   */
  private byte[] iv = null;

  /**
   * Wrapped AES key.
   */
  private byte[] wrapKey = null;

  /**
   * Fingerprint of X509 Certificate to verify client's signature.
   */
  private Fingerprint clientCertFp = null;

  /**
   * Private key used for unwrapping AES key.
   */
  private PrivateKey privKey = null;

  /**
   * The server's certificate.
   */
  private X509Certificate serverCert = null;

  /**
   * Certificate table of the creator tool.
   */
  private Certificates certs = new Certificates();

  /**
   * Config file.
   */
  private Properties cfg = null;

  /**
   * Base directory.
   */
  private File base_dir = null;

  /**
   * Default constructor.
   */
  public Server() {
  }

  /**
   * Add IAIK as provider.
   */
  static {
    IAIK.addAsProvider();
  }

  /**
   * Load necessary data.
   * 
   * @param cfg
   *          config file.
   * @param base_dir
   *          base directory.
   * 
   * @throws CertificateException
   * @throws IOException
   * @throws InvalidKeyException
   * @throws NoSuchAlgorithmException
   */
  public void setup(Properties cfg, File base_dir) throws CertificateException,
      IOException, InvalidKeyException, NoSuchAlgorithmException {

    this.cfg = cfg;
    this.base_dir = base_dir;

    loadServerCertificateAndKey();
    addCertificates();

    for (int n = 0;; ++n) {
      String cert_id = cfg.getProperty("server.trusted.root." + n);
      if (cert_id == null) {
        break;
      }

      this.certs.addTrusted(new Fingerprint(cert_id));
    }
  }

  /**
   * The client uses this method to send data to the server.
   * 
   * @param ccm
   *          The signed and encrypted data.
   * @param wrapped_key
   *          The AES decryption key, wrapped with the server's public key.
   * @param iv
   *          The initial value for AES decryption.
   * @param fingerprint
   *          The fingerprint of the client's certificate.
   * 
   * @throws InvalidKeyException
   * @throws NoSuchAlgorithmException
   * @throws NoSuchProviderException
   * @throws NoSuchPaddingException
   * @throws InvalidAlgorithmParameterException
   * @throws IllegalBlockSizeException
   * @throws BadPaddingException
   * @throws SignatureException
   * @throws IOException
   * @throws CertificateException
   * @throws RevocationCheckException
   * @throws ClassNotFoundException
   * @throws InstantiationException
   * @throws IllegalAccessException
   * @throws IllegalArgumentException
   * @throws InvocationTargetException
   * @throws NoSuchMethodException
   * @throws SecurityException
   */
  public void send(byte[] ccm, byte[] wrapped_key, byte[] iv,
      Fingerprint fingerprint) throws InvalidKeyException,
      NoSuchAlgorithmException, NoSuchProviderException,
      NoSuchPaddingException, InvalidAlgorithmParameterException,
      IllegalBlockSizeException, BadPaddingException, SignatureException,
      IOException, CertificateException, ClassNotFoundException,
      InstantiationException, IllegalAccessException, IllegalArgumentException,
      InvocationTargetException, NoSuchMethodException, SecurityException {

    clientData = ccm.clone();
    wrapKey = wrapped_key.clone();
    this.iv = iv.clone();
    clientCertFp = fingerprint;

    processSignedData();
  }

  /**
   * The client uses this method to get the server's certificate.
   * 
   * @return The server's certificate.
   */
  public Fingerprint requestCertificate() {
    return new Fingerprint(this.serverCert);
  }

  /**
   * Load the server's private key and certificate from the filesystem.
   * 
   * @throws InvalidKeyException
   * @throws IOException
   * @throws CertificateException
   */
  private void loadServerCertificateAndKey() throws InvalidKeyException,
      IOException, CertificateException {

    // Load private key for signing
    String key_path = cfg.getProperty("keys.path");
    String sig_key_file = key_path + cfg.getProperty("server.key");
    if (sig_key_file != null) {
      // Full path
      sig_key_file = new File(base_dir, sig_key_file).getAbsolutePath();

      // Password?
      char[] pw_chars = null;

      String pw_str = cfg.getProperty("key.password");
      if (pw_str != null) {
        pw_chars = pw_str.toCharArray();
      }

      // Load the key
      this.privKey = CryptoToolBox.loadPemPrivateKey(sig_key_file, pw_chars);

      log("[+] server private key loaded from " + sig_key_file);
    }

    // Load certificate for signing
    String cert_path = cfg.getProperty("certificates.path");
    String server_cert = cert_path + cfg.getProperty("server.cert");
    if (server_cert != null) {
      // Full path
      server_cert = new File(base_dir, server_cert).getAbsolutePath();

      this.serverCert = CryptoToolBox.loadCertificate(server_cert);

      log("[+] server certificate loaded from " + server_cert);
      log("    with fingerprint " + new Fingerprint(this.serverCert));
    }
  }

  /**
   * Processes the data received from the client.
   * 
   * @throws InvalidKeyException
   * @throws IOException
   * @throws NoSuchAlgorithmException
   * @throws NoSuchProviderException
   * @throws NoSuchPaddingException
   * @throws InvalidAlgorithmParameterException
   * @throws IllegalBlockSizeException
   * @throws BadPaddingException
   * @throws SignatureException
   * @throws CertificateException
   * @throws RevocationCheckException
   * @throws ClassNotFoundException
   * @throws InstantiationException
   * @throws IllegalAccessException
   * @throws IllegalArgumentException
   * @throws InvocationTargetException
   * @throws NoSuchMethodException
   * @throws SecurityException
   */
  private void processSignedData() throws InvalidKeyException, IOException,
      NoSuchAlgorithmException, NoSuchProviderException,
      NoSuchPaddingException, InvalidAlgorithmParameterException,
      IllegalBlockSizeException, BadPaddingException, SignatureException,
      CertificateException, ClassNotFoundException, InstantiationException,
      IllegalAccessException, IllegalArgumentException,
      InvocationTargetException, NoSuchMethodException, SecurityException {

    // Check the client's certificate.
    X509Certificate clientCert = certs.use(clientCertFp,
        IntendedUsage.SIGNATURE);

    // Unwrap the AES key.
    SecretKey aes_unwrapped = CryptoToolBox.unWrapKey(wrapKey,
        CryptoToolBox.ENC_ALG_RSA_ECB_PKCS1, privKey);

    // Decrypt the received data.
    byte[] decrypted = CryptoToolBox.aesCcmDecrypt(clientData, 0,
        clientData.length, aes_unwrapped, iv);

    // Unpack the decrypted datablob and check it's signature.
    byte[] unpackedData = CryptoToolBox.verifyBlobAndUnpack(decrypted,
        clientCert.getPublicKey());

    // Pass the binary to the codlet
    Codlet kotlett = new Codlet();
    kotlett.processBinary(cfg, base_dir, unpackedData);
  }

  /**
   * Loads all certificates from the path, specified in
   * {@code config.properties} by the property {@code certificates.path}
   * 
   * @throws CertificateException
   * @throws IOException
   */
  private void addCertificates() throws CertificateException, IOException {

    String cert_path = cfg.getProperty("certificates.path");

    if (cert_path != null) {
      // Full path
      cert_path = new File(base_dir, cert_path).getAbsolutePath();

      // Load certificates
      int n = this.certs.addAllFromDirectory(cfg
          .getProperty("certificates.path"));

      log("[+] imported " + n + " certificates from " + cert_path);
    }
  }

  /**
   * Logs a console message
   * 
   * @param message
   *          Message to be printed.
   */
  public static void log(String message) {
    System.err.println(message);
  }
}