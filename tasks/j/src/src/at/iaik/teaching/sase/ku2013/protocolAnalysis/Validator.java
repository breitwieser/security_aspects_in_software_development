package at.iaik.teaching.sase.ku2013.protocolAnalysis;

import iaik.x509.X509Certificate;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.security.cert.CertificateException;
import java.util.Properties;

import at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox;
import at.iaik.teaching.sase.ku2013.crypto.Fingerprint;

public final class Validator {

  public static void main(String[] args) throws IOException,
      ResourceCheckException, ClassNotFoundException, InstantiationException,
      IllegalAccessException, IllegalArgumentException,
      InvocationTargetException, NoSuchMethodException, SecurityException,
      CertificateException {

    // File setup
    File config_file = new File("validator.properties").getAbsoluteFile();

    Properties cfg = new Properties();

    FileInputStream in = new FileInputStream(config_file);
    try {
      cfg.load(in);
    } finally {
      in.close();
    }
    String blackBoxImpl = cfg.getProperty("blackbox");
    String channelImpl = cfg.getProperty("channelImpl");
    String path = cfg.getProperty("certificates.path");

    ResourceDatabase database = createDatabase();
    CommunicationChannel channel = getChannel(channelImpl);
    ResourceChecker blackbox = getBlackBox(blackBoxImpl, database, channel,
        cfg);

    if (cfg.getProperty("validator.run.multiple").equals("true"))
      for (int n = 0;; ++n) {
        String cert = cfg.getProperty("validator.cert." + n);
        if (cert == null) {
          System.exit(0);
        }
        X509Certificate certificate = CryptoToolBox
            .loadCertificate(path + cert);
        System.out.println("***** CHECKING CERTIFICATE: "
            + new Fingerprint(certificate));
        ResourceCheckResult response = blackbox.check(certificate);
        System.out.println("Certificate status from server: " + response);
        System.out.println();
      }

    String cert = path + cfg.getProperty("validator.cert.0");
    X509Certificate certificate = CryptoToolBox.loadCertificate(cert);
    ResourceCheckResult response = blackbox.check(certificate);
    System.out.println("Certificate status from server: " + response);
  }

  private static ResourceDatabase createDatabase() throws IOException {
    return new ResourceDbImpl();
  }

  private static CommunicationChannel getChannel(String name)
      throws ClassNotFoundException, InstantiationException,
      IllegalAccessException, IllegalArgumentException,
      InvocationTargetException, NoSuchMethodException, SecurityException {
    Class<?> cl = Class.forName(name);
    CommunicationChannel ch = (CommunicationChannel) cl.getConstructor()
        .newInstance();
    return ch;
  }

  private static ResourceChecker getBlackBox(String name,
      ResourceDatabase db, CommunicationChannel ch, Properties cfg)
      throws InstantiationException, IllegalAccessException,
      IllegalArgumentException, InvocationTargetException,
      NoSuchMethodException, SecurityException, ClassNotFoundException {
    Class<?> cl = Class.forName(name);
    ResourceChecker checker = null;

    checker = (ResourceChecker) cl.getConstructor(ResourceDatabase.class,
        CommunicationChannel.class).newInstance(db, ch);

    return checker;
  }
}
