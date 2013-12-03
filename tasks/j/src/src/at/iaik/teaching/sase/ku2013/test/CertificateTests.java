package at.iaik.teaching.sase.ku2013.test;

import static iaik.utils.Util.decodeByteArray;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;
import iaik.pkcs.PKCS7CertList;
import iaik.security.provider.IAIK;
import iaik.x509.X509Certificate;
import iaik.x509.extensions.BasicConstraints;
import iaik.x509.extensions.KeyUsage;

import java.io.ByteArrayInputStream;
import java.io.FileInputStream;
import java.security.cert.CertificateException;

import org.junit.Before;
import org.junit.Test;

import at.iaik.teaching.sase.ku2013.crypto.Certificates;
import at.iaik.teaching.sase.ku2013.crypto.Fingerprint;
import at.iaik.teaching.sase.ku2013.crypto.IntendedUsage;

public class CertificateTests {

  /**
   * Base64 encoded PKCS#7 certificate bundle with the certificate chain of the
   * example "The Device.crt" certificate.
   */
  private static final String B64_PKCS7_DEVICE_CERT = "MIINywYJKoZIhvcNAQcCoIINvDCCDbgCAQExADALBgkqhkiG9w0BBwGggg2gMIID"
      + "PTCCAiWgAwIBAgIBBjANBgkqhkiG9w0BAQUFADBfMRQwEgYDVQQKEwtUaGUgQ29t"
      + "cGFueTEnMCUGA1UECxMeQ2VydGlmaWNhdGUgQXV0aG9yaXR5IFNlcnZpY2VzMR4w"
      + "HAYDVQQDExVEZXZpY2UgRW5kb3JzZW1lbnQgQ0EwHhcNMTIxMjE4MTMzOTAwWhcN"
      + "MTMxMjE4MTMxNTAwWjBDMRUwEwYDVQQKEwxUaGUgQ3VzdG9tZXIxFTATBgNVBAsT"
      + "DFRoZSBEaXZpc2lvbjETMBEGA1UEAxMKVGhlIERldmljZTCCASIwDQYJKoZIhvcN"
      + "AQEBBQADggEPADCCAQoCggEBAM07A6iZNLzqD+7JSH7Zx7HJBNSlbvls0w6sPw8g"
      + "XLG82gDgGHZBlgaAWe6yb6jdoTSRFfVhRlNm82bQt6CcDr+Nr4s6LejLT8Emny87"
      + "dOri4n9elrjJYLfuCZ+gw44tITMnHBEzG526vjm1RY980JPLsJ5axV9PX1JjjmSb"
      + "EGrdi+Uj+P31uYCi56543Qhm8Nu7Hmnm/+hpdvnMofYJDhBv30srh2mwfkJog3ku"
      + "ewQgZisQgLwGtwGCD8z6+M1XTXbHblFApWrLNOK7CIDr0Ix4hKKtIWMAU0YQpYDK"
      + "PoX9zToMdXu6kZFl4GDgSgX37vnST8FzSFCqfA8b8dJtUCUCAwEAAaMgMB4wDAYD"
      + "VR0TAQH/BAIwADAOBgNVHQ8BAf8EBAMCBSAwDQYJKoZIhvcNAQEFBQADggEBAEv/"
      + "U3BhvPaHuMZ498O1V4bS+vlu4wrvnGndDYADj5L0URmmHZh4EMAmZP2iI0FoUdgh"
      + "k+f47d59pljWiW0RObRLPN9u91G4OPxnnwbw4WWMHHuOlaMiKowmeEwYyv7J7TXl"
      + "gYTLAMx1XCXsz7UKCI6aB70szrET1ZlmAZQ/u5vFaGWuLjITDIor9q9js0M0e2oz"
      + "9it63SfHKPI8ldp4jd+QfRLQXLPz/H2IgWstcf5MXVGoAN/EpPek8HEb9BEppKSo"
      + "+J6jGzc2G6bSdcq/JMeXLXhvi2pIuigUo745Iu08qKvrGYcWGbUcRFEprHGu8woB"
      + "Q2RiQrPG65pHs7AUKLEwggN3MIICX6ADAgECAgEFMA0GCSqGSIb3DQEBBQUAMF8x"
      + "FDASBgNVBAoTC1RoZSBDb21wYW55MScwJQYDVQQLEx5DZXJ0aWZpY2F0ZSBBdXRo"
      + "b3JpdHkgU2VydmljZXMxHjAcBgNVBAMTFUludGVybWVkaWF0ZSBMZXZlbCBDQTAe"
      + "Fw0xMjEyMTgxMzM1MDBaFw0xMzEyMTgxMzE1MDBaMF8xFDASBgNVBAoTC1RoZSBD"
      + "b21wYW55MScwJQYDVQQLEx5DZXJ0aWZpY2F0ZSBBdXRob3JpdHkgU2VydmljZXMx"
      + "HjAcBgNVBAMTFURldmljZSBFbmRvcnNlbWVudCBDQTCCASIwDQYJKoZIhvcNAQEB"
      + "BQADggEPADCCAQoCggEBAOnVPAoriMAfIcPkxm6rlc7sR1ysv4R3Oj9mZSNT8DCa"
      + "LC8lokPMq5Ym+65in+vCf0xn94iuFUImfb3yy/3zcSs9HCaUpk8Qgrswe2kPn4fH"
      + "6xYRtkq6rn6FF+DyUlNiybJ39NTJ7aoWRxom70+f0wtJHhLsMzH8FOY4gpSjLJ8j"
      + "wsm23Upg/PZSyZsdeSVoP34x+DhFni2WpOaQAzMZEBDSGqVa7b0X3MwGvPaTOdtB"
      + "RRISSM8m/tsW+5g/1RSlfDa7m9K/nIVFYvNQIE4eAsZMXeRWmlJ3xSOll5Vz7hCm"
      + "OYAAyAiFb9JlnQvaHVr2QL6QyTPD1jRuW4lKH+poHDMCAwEAAaM+MDwwEgYDVR0T"
      + "AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAQYwFgYDVR0lAQH/BAwwCgYIKwYB"
      + "BQUHAwkwDQYJKoZIhvcNAQEFBQADggEBAJX0EiIeaQsIcmmq5PINvgExnEtqYx2p"
      + "36KACvWWAyoVIhLp2DuvVt3IVZQv6z0ebE21096tEKqJxN6dQUNTo4F9q/V3LIxx"
      + "7c+CmVm4l8f80IB+uA51w0djXhLHK+nIZGkhGkgHK6b8YLnAaWM40P4OZ/DXTCZ2"
      + "QFb5fIJeaDNqVqjJvIyl/8DZycbGGwuHyVFqVHsF1Cv2lHiqcg2Odj7j4wxE4faP"
      + "cHwjCabMzuVN036mfdbMGJGD5YcbrjP6+miMHpC7CEu7Wiez82ZMifc1eW/qIO7z"
      + "kRraJ8CjXVFAI7yrDwCsOT1f/2pQL3L3E5S5Hz6VYQC54BVnlXS60kAwggNxMIIC"
      + "WaADAgECAgEDMA0GCSqGSIb3DQEBBQUAMFkxFDASBgNVBAoTC1RoZSBDb21wYW55"
      + "MScwJQYDVQQLEx5DZXJ0aWZpY2F0ZSBBdXRob3JpdHkgU2VydmljZXMxGDAWBgNV"
      + "BAMTD1RydXN0ZWQgUm9vdCBDQTAeFw0xMjEyMTgxMzIzMDBaFw0xMzEyMTgxMzE1"
      + "MDBaMF8xFDASBgNVBAoTC1RoZSBDb21wYW55MScwJQYDVQQLEx5DZXJ0aWZpY2F0"
      + "ZSBBdXRob3JpdHkgU2VydmljZXMxHjAcBgNVBAMTFUludGVybWVkaWF0ZSBMZXZl"
      + "bCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANNtXi10HEjoPuWx"
      + "JEFKX0x3z0FmeDKzyHhfi43+nt5xRDA4/CKE2PzxMj6QcycZpNk3G7At5+lyjxkt"
      + "qEHN5C4cLmMWf+Az72rR/ofQKk+s/6SoUrSWo6LWSLrdp0c7bp7ZMlqLvkLpuXDh"
      + "IcloGos3x7XuNr7NozYho9JijO3CyEpOH28Pgqr+RlBRN0sdCiUkh4Ob29RohRIn"
      + "acJ8TqQi0vf/hbDXzD+yuQ4KoBSNgz8/B/Kn/n7l9GLSM6dkCdBDflFIm6BSGjWK"
      + "QnaztoD//iCauy2aDFyIT4s3SbDd3OzlodXWN6aUTDdo/Pkvp607Nsj4z0cl19St"
      + "vILHDEECAwEAAaM+MDwwEgYDVR0TAQH/BAgwBgEB/wIBATAOBgNVHQ8BAf8EBAMC"
      + "AQYwFgYDVR0lAQH/BAwwCgYIKwYBBQUHAwkwDQYJKoZIhvcNAQEFBQADggEBALtt"
      + "H/qVwA1nnhekf9lSiLunuegxU9VnIgtT0LYsPB0dc3a3a2X+Mzdyxe03uaMtHaD5"
      + "Wml+sHs7WVAmONuLMizJVaZW73KlxfNplpaohVua6VCKWqHlxhaaD4GkCro1czxB"
      + "pCg9ZzI49XoqKyKQNoD0RBVdH3SqQG00+YAzV4M+KtTYMqnkkocYEhPGX3aLAmp3"
      + "iZf18Z7SKdDM50KGCUfUx9CtTQFHUrR+MsGEMSyY6qKKoudkrhgD/QcpkmDHlAMv"
      + "iulKjLLAxffvnNS55IAi+4NN/hjY4c7MYQrqz6J2cOvozvDTSImKqXunYmxq7pGg"
      + "SCw+brUB4pH1izVYJcYwggNrMIICU6ADAgECAgEBMA0GCSqGSIb3DQEBBQUAMFkx"
      + "FDASBgNVBAoTC1RoZSBDb21wYW55MScwJQYDVQQLEx5DZXJ0aWZpY2F0ZSBBdXRo"
      + "b3JpdHkgU2VydmljZXMxGDAWBgNVBAMTD1RydXN0ZWQgUm9vdCBDQTAeFw0xMjEy"
      + "MTgxMzE1MDBaFw0xMzEyMTgxMzE1MDBaMFkxFDASBgNVBAoTC1RoZSBDb21wYW55"
      + "MScwJQYDVQQLEx5DZXJ0aWZpY2F0ZSBBdXRob3JpdHkgU2VydmljZXMxGDAWBgNV"
      + "BAMTD1RydXN0ZWQgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC"
      + "ggEBAM3uW/R4kQFWV9CR3FiYd3mWZ9NSgnr7RPr5NPUONIuIBcPv8Naq2WY5qGcC"
      + "Fmg9ZiR/9COFa5rZTZH1yb5/MPLtUeebq/TlcNHmRrvB87blFZZFbUgEO/WKSyFY"
      + "0ckL9d7dpKmbrxOpGA35ubmbMHBcOMyJlXMKAK8aPeWUoaFdlfc+pIoyzcAs03vV"
      + "4kyINhcZKkXBMtyI9DqAHtwGPTW0lj9yMXdSTz0dmVSkyCNbDOX96KuNzpQTDKVb"
      + "451+uYHh34LmkU3mHTkW4vqtGCqyuJzGUFTzIfZfbaLvRVS/jmcDpPG4qXRjd/Vo"
      + "EBlPGpBicvwl3LsshjmyER65Ef0CAwEAAaM+MDwwEgYDVR0TAQH/BAgwBgEB/wIB"
      + "AjAOBgNVHQ8BAf8EBAMCAQYwFgYDVR0lAQH/BAwwCgYIKwYBBQUHAwkwDQYJKoZI"
      + "hvcNAQEFBQADggEBACikjk1tpOzK6GgRFaEH2+lojoGlZomm1Pj8uDurZRkuAiJG"
      + "v4AhAvhS3Kyk7l4Mejky1c7tz/ECJrLM9y9NWzRDBgiZQZf5mB0GGTFyX7NFRGm2"
      + "i7m+y7yVhXD/fpTbKI0Nfnr56iMXl1b5/cXYkoEh7TrBUn9iv6s8Gz42P5qFw1Vw"
      + "nthVb991SgknZQR8M6iydaUMxRLFBASrlK79EYreunVWGk8gdDRZCRZ0xuanHFSX"
      + "XY+1uTl+2WaGJoU0Ulm1As+Qy8WCPLljm2d53lhCD02ESOQ5um1ExULr2N6Xy2qi"
      + "UAXNe0dumkZFtXI3gxcLQ/mRkFjK1ajEHaaLeoYxAA==";

  private static X509Certificate[] loadCerts(String bundle_data)
      throws Exception {
    ByteArrayInputStream in = new ByteArrayInputStream(
        decodeByteArray(bundle_data));
    PKCS7CertList bundle = new PKCS7CertList(in);
    return bundle.getCertificateList();
  }

  @Before
  public void setUp() {
    IAIK.addAsProvider();
  }

  // Test helper for key usage tests.
  private static boolean testKeyUsage(IntendedUsage usage, boolean is_ca,
      int path_len, int key_usage) {
    BasicConstraints basic = new BasicConstraints(is_ca, path_len);
    return usage.isAllowedBy(basic, new KeyUsage(key_usage));

  }

//  @Test
  public void testCaUsage() {
    // Simple valid CA attributes
    assertTrue(testKeyUsage(IntendedUsage.CA, true, 0, KeyUsage.keyCertSign));
    assertTrue(testKeyUsage(IntendedUsage.CA, true, 1, KeyUsage.keyCertSign));
    assertTrue(testKeyUsage(IntendedUsage.CA, true, Integer.MAX_VALUE,
        KeyUsage.keyCertSign));
  }

//  @Test
  public void testCaUsageWithAdditionalKeyUsage() {
    // Simple valid CA attributes (additional key uses)
    assertTrue(testKeyUsage(IntendedUsage.CA, true, 0, KeyUsage.keyCertSign
        | KeyUsage.cRLSign));
    assertTrue(testKeyUsage(IntendedUsage.CA, true, 1, KeyUsage.keyCertSign
        | KeyUsage.cRLSign));
    assertTrue(testKeyUsage(IntendedUsage.CA, true, Integer.MAX_VALUE,
        KeyUsage.keyCertSign | KeyUsage.cRLSign));

    // CA with signature capabilities ... :)
    assertTrue(testKeyUsage(IntendedUsage.CA, true, 1, KeyUsage.keyCertSign
        | KeyUsage.digitalSignature));

  }

//  @Test
  public void testCaUsageNotCA() {
    // Simple invalid CA attributes (CA flag is false)
    assertFalse(testKeyUsage(IntendedUsage.CA, false, 0, KeyUsage.keyCertSign));
    assertFalse(testKeyUsage(IntendedUsage.CA, false, 1, KeyUsage.keyCertSign));
    assertFalse(testKeyUsage(IntendedUsage.CA, false, Integer.MAX_VALUE,
        KeyUsage.keyCertSign));
  }

//  @Test
  public void testCaUsageCAButWrongKeyUsage() {
    // Simple invalid CA attributes (CA flag is true but wrong key usage
    // attributes)
    assertFalse(testKeyUsage(IntendedUsage.CA, true, 0,
        KeyUsage.digitalSignature));
    assertFalse(testKeyUsage(IntendedUsage.CA, true, 1,
        KeyUsage.digitalSignature));
    assertFalse(testKeyUsage(IntendedUsage.CA, true, Integer.MAX_VALUE,
        KeyUsage.digitalSignature));
  }

//  @Test
  public void testSignUsage() {
    // Simple valid signature attributes ...
    assertTrue(testKeyUsage(IntendedUsage.SIGNATURE, false, 0,
        KeyUsage.digitalSignature));

    // CA with signature capabilities ... :)
    assertTrue(testKeyUsage(IntendedUsage.SIGNATURE, true, 1,
        KeyUsage.keyCertSign | KeyUsage.digitalSignature));
  }

//  @Test
  public void testSignUsageWrong() {
    // CA without digital signature usage ...
    assertFalse(testKeyUsage(IntendedUsage.SIGNATURE, true, 0,
        KeyUsage.keyCertSign));

    // Key wrapping and encryption only ...
    assertFalse(testKeyUsage(IntendedUsage.SIGNATURE, false, 0,
        KeyUsage.dataEncipherment | KeyUsage.keyEncipherment));

    // Key wrapping only
    assertFalse(testKeyUsage(IntendedUsage.SIGNATURE, false, 0,
        KeyUsage.keyEncipherment));
  }

//  @Test
  public void testSignUsageOneCert() throws Exception {
    X509Certificate[] raw_certs = loadCerts(B64_PKCS7_DEVICE_CERT);

    // Finger-prints of the (ordered!) test chain.
    Fingerprint root_id = new Fingerprint(raw_certs[raw_certs.length - 1]);
    Fingerprint leaf_id = new Fingerprint(raw_certs[0]);

    // Setup the incomplete chain
    Certificates certs = new Certificates();

    // Set the trusted root id (no certificate is added!)
    certs.addTrusted(root_id);

    // Only add the leaf certificate
    certs.add(raw_certs[0]);

    // Try to verify usage of the signature certificate
    try {
      // And try to verify it (this should fail!)
      certs.use(leaf_id, IntendedUsage.SIGNATURE);

      // Huh? Something got messed up ...
      fail("Signature certificate verified with empty chain (HUH?)");

    } catch (CertificateException e) {
      // Pass, expected result is verification failure
    }

  }

//  @Test
  public void testSignUsageIncompleteChain() throws Exception {
    X509Certificate[] raw_certs = loadCerts(B64_PKCS7_DEVICE_CERT);

    // Finger-prints of the (ordered!) test chain.
    Fingerprint root_id = new Fingerprint(raw_certs[raw_certs.length - 1]);
    Fingerprint leaf_id = new Fingerprint(raw_certs[0]);

    // Test incomplete certificate chains, where one certifcate
    // along the path is missing.
    //
    // The leaf-certificate is always in the table. Other certs
    // may be missing.
    for (int n = 1; n < raw_certs.length; ++n) {

      // Setup the incomplete chain
      Certificates certs = new Certificates();

      // Set the trusted root.
      certs.addTrusted(root_id);

      // Add all certificates except the certificate at index n
      for (int k = 0; k < raw_certs.length; ++k) {
        if (n != k) {
          certs.add(raw_certs[k]);
        }
      }

      // Try to verify usage of the signature certificate
      try {
        // And try to verify it (this should fail!)
        certs.use(leaf_id, IntendedUsage.SIGNATURE);

        // Huh? Something got messed up ...
        fail("Incomplete chain with missing certificate #" + n
            + " is not valid!");

      } catch (CertificateException e) {
        // Pass, expected result is verification failure
      }
    }
  }

//  @Test
  public void testVerifySimple() throws Exception {
    X509Certificate[] raw_certs = loadCerts(B64_PKCS7_DEVICE_CERT);
    Fingerprint[] fingerprints = new Fingerprint[raw_certs.length];

    // Prepare the finger-prints
    for (int n = 0; n < raw_certs.length; ++n) {
      fingerprints[n] = new Fingerprint(raw_certs[n]);
    }

    // Create a new certificate table and add our certificates
    Certificates certs = new Certificates();

    // Mark the root certificate as trusted (The certificate is not
    // yet in the table.)
    certs.addTrusted(fingerprints[raw_certs.length - 1]);

    // Populate the certificate table with our test certificates
    for (X509Certificate cert : raw_certs) {
      certs.add(cert);
    }

    // Only the root certificate is trusted
    for (int n = 0; n < raw_certs.length - 1; ++n) {
      assertFalse(certs.isTrusted(fingerprints[n]));
    }

    assertTrue(certs.isTrusted(fingerprints[raw_certs.length - 1]));

    // Try to access the certificates without usage checks (should always
    // succeed,
    // irrespective of the validity and/or completeness of the chain)
    for (int n = 1; n < raw_certs.length; ++n) {
      assertSame(raw_certs[n], certs.use(fingerprints[n], null));
    }

    // The non-leaf certificates are CA certificates
    for (int n = 1; n < raw_certs.length; ++n) {
      X509Certificate cert = certs.use(fingerprints[n], IntendedUsage.CA);

      // Must be the same object
      assertSame(raw_certs[n], cert);
    }

    // The leaf certificate is a CA certificate
    assertSame(raw_certs[0], certs.use(fingerprints[0], IntendedUsage.WRAP_KEY));
  }
  
  @Test
  public void testVerifySimple2() throws Exception {
	System.out.println("testVerifySimple2");
	FileInputStream in = new FileInputStream("examples/multipath/Leaf_1.p7b");
	PKCS7CertList bundle = new PKCS7CertList(in);
	X509Certificate[] raw_certs = bundle.getCertificateList();
    Fingerprint[] fingerprints = new Fingerprint[raw_certs.length];
    

    // Prepare the finger-prints
    for (int n = 0; n < raw_certs.length; ++n) {
      fingerprints[n] = new Fingerprint(raw_certs[n]);
    }
// 	  0) Root 2 CA
//    1) Intermediate 2 CA
//    2) Leaf
//    3) Root 1 CA
//    4) Intermediate 1 CA
//    5) Leaf
// Tree:
//    0			3
//     \	   /
//		1	  4
//		 \   /
//		 2 = 5
    
    // Create a new certificate table and add our certificates
    Certificates certs = new Certificates();

    // Mark the root certificate as trusted (The certificate is not
    // yet in the table.)
    // hardcoded
    certs.addTrusted(fingerprints[0]); // Root 2 CA
    certs.addTrusted(fingerprints[3]); // Root 1 CA
    // Populate the certificate table with our test certificates
    for (X509Certificate cert : raw_certs) {
      certs.add(cert);
    }

    assertTrue(certs.isTrusted(fingerprints[0])); // Root 2 CA
    assertTrue(certs.isTrusted(fingerprints[3])); // Root 1 CA

    // The non-leaf certificates are CA certificates
    for (int n = 0; n < raw_certs.length; ++n) {
    	if(n != 2 && n != 5)
    	{
	      X509Certificate cert = certs.use(fingerprints[n], IntendedUsage.CA);
	
	      // Must be the same object
	      assertSame(raw_certs[n], cert);
    	}
    }

    // The leaf certificate is a CA certificate
    assertSame(raw_certs[2], certs.use(fingerprints[2], IntendedUsage.WRAP_KEY));
    assertSame(raw_certs[5], certs.use(fingerprints[5], IntendedUsage.WRAP_KEY));
    
    System.out.println("end testVerifySimple2");
  }

//  @Test
  public void testVerifyErrors() throws Exception {
    X509Certificate[] raw_certs = loadCerts(B64_PKCS7_DEVICE_CERT);

    Fingerprint root_id = new Fingerprint(raw_certs[raw_certs.length - 1]);

    // Create a new certificate table and add our certificates
    Certificates certs = new Certificates();

    // Populate the certificate table with our test certificates
    for (X509Certificate cert : raw_certs) {
      certs.add(cert);
    }

    // No certificates are trusted
    for (int n = 0; n < raw_certs.length; ++n) {
      assertFalse(certs.isTrusted(raw_certs[n]));
    }

    // TEST 1: Verification must fail for each certificate (no trusted root
    // path)
    for (int n = 0; n < raw_certs.length; ++n) {
      try {
        Fingerprint id = new Fingerprint(raw_certs[n]);
        IntendedUsage usage = n > 0 ? IntendedUsage.CA : IntendedUsage.WRAP_KEY;
        certs.use(id, usage);
        fail("Verification without trusted root MUST fail.");
      } catch (CertificateException e) {
        // Pass, expected error
      }
    }

    // Mark the root certificate as trusted
    certs.addTrusted(root_id);

    // TEST 2: Verification must succeed for each certificate
    // (path to trusted root is available, chain is valid)
    for (int n = 0; n < raw_certs.length; ++n) {
      Fingerprint id = new Fingerprint(raw_certs[n]);
      IntendedUsage usage = n > 0 ? IntendedUsage.CA : IntendedUsage.WRAP_KEY;
      X509Certificate used_cert = certs.use(id, usage);
      assertSame(raw_certs[n], used_cert);
    }
  }

  //@Test
  public void testVerifyErrorsTampering() throws Exception {
    X509Certificate[] raw_certs = loadCerts(B64_PKCS7_DEVICE_CERT);

    // Grab the certificate at index 2 and tamper with its representation.
    X509Certificate good_cert = raw_certs[1];
    byte[] tampered_bytes = good_cert.toByteArray();

    // Subject name of the selected test certificate starts at byte offset
    // 0x0EC. Let's tamper with it!
    tampered_bytes[0x0EC] = 'H';
    tampered_bytes[0x0ED] = 'A';
    tampered_bytes[0x0EE] = 'C';
    tampered_bytes[0x0EF] = 'K';
    tampered_bytes[0x0F0] = 'E';
    tampered_bytes[0x0F1] = 'D';
    tampered_bytes[0x0F2] = '!';

    // And patch it in
    raw_certs[1] = new X509Certificate(tampered_bytes);

    // Check the validity (not valid before/after) only! this does not
    // involve
    // a signature check ...
    raw_certs[1].checkValidity();

    // Create a new certificate table and add our certificates
    Certificates certs = new Certificates();

    // Populate the certificate table with our test certificates
    for (X509Certificate cert : raw_certs) {
      certs.add(cert);
    }

    // Mark the root certificate as trusted
    certs.addTrusted(new Fingerprint(raw_certs[raw_certs.length - 1]));

    // TEST 1: Certificates must be accessible without verification.
    for (int n = 0; n < raw_certs.length; ++n) {
      Fingerprint id = new Fingerprint(raw_certs[n]);
      X509Certificate cert = certs.use(id, null);
      assertSame(raw_certs[n], cert);
    }

    // TEST 2: We have tampered with the certificate at raw_certs[1], so
    // signature
    // verification will fail during chain verification at raw_certs[0] and
    // raw_certs[1]. The certificates further up in the chain are
    // unaffected.
    for (int n = 0; n < raw_certs.length; ++n) {
      try {
        Fingerprint id = new Fingerprint(raw_certs[n]);
        IntendedUsage usage = n > 0 ? IntendedUsage.CA : IntendedUsage.WRAP_KEY;
        X509Certificate used_cert = certs.use(id, usage);

        assertTrue(n >= 2);
        assertSame(raw_certs[n], used_cert);

      } catch (CertificateException e) {
        // Verification error, assert that we are talking about
        // certificate #0 or #1
        assertTrue(n == 0 || n == 1);
      }
    }
  }
}
