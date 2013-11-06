package at.iaik.teaching.sase.ku2013.client;

import static iaik.utils.Util.readFile;
import iaik.security.provider.IAIK;
import iaik.x509.X509Certificate;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PrivateKey;
import java.security.SignatureException;
import java.security.cert.CertificateException;
import java.security.spec.InvalidKeySpecException;
import java.util.Properties;

import javax.crypto.BadPaddingException;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.SecretKey;

import at.iaik.teaching.sase.ku2013.crypto.Certificates;
import at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox;
import at.iaik.teaching.sase.ku2013.crypto.Fingerprint;
import at.iaik.teaching.sase.ku2013.crypto.IntendedUsage;
import at.iaik.teaching.sase.ku2013.server.Server;

/**
 * The client takes a binary signs it using the KEM/DEM mechanism 
 * and sends it to a server.
 * 
 * @author SASE 2013 Team
 *
 */
/**
 * @author SASE 2013 Team
 * 
 */
public final class Client {

  /**
   * Base directory.
   */
  File base_dir = null;

  /**
   * Config file.
   */
  Properties cfg = null;

  /**
   * Root certificates known by the client.
   */
  Properties server = null;

  /**
   * Certificate table of the creator tool.
   */
  private Certificates certs = new Certificates();

  /**
   * Binary to be signed and sent to server.
   */
  private byte[] binary = null;

  /**
   * Certificate for the client's signature key.
   */
  private X509Certificate sigCert = null;

  /**
   * Signature key of the client.
   */
  private PrivateKey sigKey = null;

  /**
   * Server certificate used for key wrapping.
   */
  private X509Certificate serverCert = null;

  /**
   * HMAC key bytes used for storing server's HMACed fingerprints
   */
  private byte[] hmacBytes = { 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
      00, 00, 00, 00, 00, 00, 00, 00 };

  /**
   * Default constructor
   * 
   * @throws IOException
   */
  public Client(String config, String roots) throws IOException {
    cfg = new Properties();
    server = new Properties();

    loadConfigFile(config, cfg);
    loadConfigFile(roots, server);
  }

  /**
   * Add IAIK as provider.
   */
  static {
    IAIK.addAsProvider();
  }

  /**
   * Load a certificate/private key pair and use it for signing and encrypting
   * the binary. Send it to the server and wait for a response.
   * 
   * @throws IOException
   * @throws CertificateException
   * @throws InvalidKeyException
   * @throws NoSuchProviderException
   * @throws NoSuchAlgorithmException
   * @throws SignatureException
   * @throws BadPaddingException
   * @throws IllegalBlockSizeException
   * @throws NoSuchPaddingException
   * @throws InvalidAlgorithmParameterException
   * @throws InvalidKeySpecException
   * @throws RevocationCheckException
   * @throws SecurityException
   * @throws NoSuchMethodException
   * @throws InvocationTargetException
   * @throws IllegalArgumentException
   * @throws IllegalAccessException
   * @throws InstantiationException
   * @throws ClassNotFoundException
   * 
   */
  public static void main(String[] args) throws IOException,
      InvalidKeyException, CertificateException, NoSuchAlgorithmException,
      NoSuchProviderException, InvalidKeySpecException, SignatureException,
      InvalidAlgorithmParameterException, NoSuchPaddingException,
      IllegalBlockSizeException, BadPaddingException, ClassNotFoundException,
      InstantiationException, IllegalAccessException, IllegalArgumentException,
      InvocationTargetException, NoSuchMethodException, SecurityException {
    
    // SETUP
    String config_filename = "config.properties";
    String roots_filename = "known.server.certs";

    Client client = new Client(config_filename, roots_filename);
    Server server = new Server();

    client.setup();
    server.setup(client.cfg, client.base_dir);

    Fingerprint serverFp = server.requestCertificate();
    if (!client.checkServerCertificate(serverFp)) {
      System.out.println("HMAC verification of server certificate failed.");
      System.exit(1);
    }

    // verify the server's certificate
    client.serverCert = client.certs.use(serverFp, IntendedUsage.WRAP_KEY);

    // create an AES key for encryption
    SecretKey aes_key = CryptoToolBox.generateSecretKey(
        CryptoToolBox.KEY_ALG_AES, CryptoToolBox.AES_KEY_LEN);

    // get the initial value for the AES encryption
    byte[] iv = CryptoToolBox.getRandomBytes(CryptoToolBox.AES_CCM_IV_SIZE);

    // sign the binary with the private key
    byte[] signed_data = CryptoToolBox.packAndSignBlob(client.binary,
        client.sigKey);

    // encrypt the signed data using AES in CCM mode
    byte[] ccmData = CryptoToolBox.aesCcmEncrypt(signed_data, 0,
        signed_data.length, aes_key, iv);

    // wrap the AES key with the server's public key
    byte[] wrapped_key = CryptoToolBox.wrapKey(aes_key,
        client.serverCert.getPublicKey());

    // obtain the fingerprint of the client's certificate
    Fingerprint fp = new Fingerprint(client.sigCert);

    // send the data to the server
    server.send(ccmData, wrapped_key, iv, fp);
  }

  /**
   * Loads necessary data from files.
   * 
   * @throws CertificateException
   * @throws IOException
   * @throws InvalidKeyException
   */
  public void setup() throws CertificateException, IOException,
      InvalidKeyException {

    readBinary();
    addCertificates();
    loadSignatureKey();
    loadClientCertificate();

    for (int n = 0;; ++n) {
      String cert_id = cfg.getProperty("client.trusted.root." + n);
      if (cert_id == null) {
        break;
      }
      this.certs.addTrusted(new Fingerprint(cert_id));
    }
  }

  /**
   * Reads a binary specified in the config file {@code config.properties} by
   * the property {@code client.binary}.
   * 
   * @throws IOException
   */
  private void readBinary() throws IOException {

    // Load binary
    String binary_file = cfg.getProperty("client.binary");
    if (binary_file != null) {
      this.binary = readFile(binary_file);
      log("[+] binary loaded from " + binary_file);
    }
  }

  /**
   * Loads the client's private key for signing a data blob.
   * 
   * @throws InvalidKeyException
   * @throws IOException
   */
  private void loadSignatureKey() throws InvalidKeyException, IOException {

    // Load private key for signing
    String key_path = cfg.getProperty("keys.path");
    String sig_key_file = key_path + cfg.getProperty("client.key");
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
      this.sigKey = CryptoToolBox.loadPemPrivateKey(sig_key_file, pw_chars);

      log("[+] signature key loaded from " + sig_key_file);
    }
  }

  /**
   * Loads the client certificate specified in the config file
   * {@code config.properties} by the properties {@code certificates.path} and
   * {@code client.cert}.
   * 
   * @throws CertificateException
   * @throws IOException
   */
  private void loadClientCertificate() throws CertificateException, IOException {

    String cert_path = cfg.getProperty("certificates.path");
    String client_cert = cert_path + cfg.getProperty("client.cert");
    if (client_cert != null) {
      // Full path
      client_cert = new File(base_dir, client_cert).getAbsolutePath();

      this.sigCert = CryptoToolBox.loadCertificate(client_cert);

      log("[+] client certificate loaded from " + client_cert);
      log("    with fingerprint " + new Fingerprint(this.sigCert));
    }
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
   * Loads the config file specified by {@code filename} to the Properties file
   * {@code config}.
   * 
   * @param filename
   *          the filename of the config file.
   * @param config
   *          the Properties file to store.
   * @throws IOException
   */
  private void loadConfigFile(String filename, Properties config)
      throws IOException {

    // File setup
    File config_file = new File(filename).getAbsoluteFile();

    base_dir = config_file.getParentFile();

    // Load the properties file
    // config = new Properties();

    FileInputStream in = new FileInputStream(config_file);
    try {
      config.load(in);
    } finally {
      in.close();
    }
  }

  /**
   * Computes the Base64 encoded SHA1-HMAC over the given Fingerprint {@code fp}
   * and compares it to the values stored in the config file
   * {@code known.server.certs}. If a proper entry is found, the client tries to
   * verify the HMAC. Else the new HMAC is stored in the
   * {@code known.server.certs} file.
   * 
   * @param fp
   * @return
   * @throws IOException
   * @throws NoSuchProviderException
   * @throws InvalidKeySpecException
   * @throws InvalidKeyException
   * @throws NoSuchAlgorithmException
   */
  private boolean checkServerCertificate(Fingerprint fp) throws IOException,
      NoSuchProviderException, InvalidKeySpecException, InvalidKeyException,
      NoSuchAlgorithmException {

    SecretKey hmacKey = CryptoToolBox.importSecretKey(
        CryptoToolBox.KEY_ALG_HMAC_SHA1, this.hmacBytes);
    byte[] hmac = CryptoToolBox.computeHmac(fp.getDigest(), 0,
        fp.getDigest().length, hmacKey);

    String hmacStr = iaik.utils.Util.toString(hmac, " ");

    for (int n = 0;; n++) {
      String server_cert = server.getProperty("known.cert." + n);

      if (server_cert == null) {
        log("[+] New server certificate found, adding to known.server.certs");
        server.setProperty("known.cert." + n, hmacStr);

        File f = new File("known.server.certs");
        OutputStream out = new FileOutputStream(f);

        server.store(out, null);
        return true;
      }

      if (server_cert.equals(hmacStr)) {
        if (CryptoToolBox.verifyHmac(fp.getDigest(), 0, fp.getDigest().length,
            hmacKey, iaik.utils.Util.toByteArray(server_cert))) {
          log("[+] server's certificate is trusted...");
          return true;
        } else {
          log("there was an error verifying the HMAC of the certificate's fingerprint");
          return false;
        }
      }
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

  public byte[] getBinary() {
    return this.binary;
  }
}