package at.iaik.teaching.sase.ku2013.protocolAnalysis;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Properties;

import at.iaik.teaching.sase.ku2013.crypto.Fingerprint;

public class ResourceDbImpl implements ResourceDatabase {

  private Properties cert_list = null;

  public ResourceDbImpl() throws IOException {

    File certs = new File("certificates.database").getAbsoluteFile();

    cert_list = new Properties();

    FileInputStream in = new FileInputStream(certs);
    try {
      cert_list.load(in);
    } finally {
      in.close();
    }
  }

  @Override
  public ResourceCheckResult query(Fingerprint fingerprint)
      throws ResourceCheckException {
    
    String cert_status = cert_list.getProperty(fingerprint.toString().replace(":", ""));
    
    if (cert_status.equals("good"))
      return ResourceCheckResult.NotRevoked;

    else if (cert_status.equals("revoked"))
      return ResourceCheckResult.Revoked;

    return ResourceCheckResult.Revoked;
  }
}
