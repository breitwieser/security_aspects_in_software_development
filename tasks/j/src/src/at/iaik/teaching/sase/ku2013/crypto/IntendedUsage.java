package at.iaik.teaching.sase.ku2013.crypto;

import iaik.x509.extensions.BasicConstraints;
import iaik.x509.extensions.ExtendedKeyUsage;
import iaik.x509.extensions.KeyUsage;

/**
 * Intended use of a certificate and its enclosed public key.
 * 
 * Elements of this enumeration represent intended uses of the public key found
 * in an X.509 certificate.
 * 
 * A certificate typically encodes the allowed uses of the certified key in its
 * {@link BasicConstraints}, {@link KeyUsage} and {@link ExtendedKeyUsage}
 * extensions.
 * 
 * The members of this enumeration encode intended uses of the key (from an
 * application's point of view) and the policies relating these intended use
 * cases to the extensions found in a certificate.
 */
public enum IntendedUsage {
  /**
   * Certificate is intended to be used as CA certificate.
   */
  CA {
    @Override
    public boolean isAllowedBy(BasicConstraints basic, KeyUsage usage) {
      // TODO: Check that the basic constraints indicate a CA certificate,
      // with a path length constraint larger than zero. Additionally
      // check that the key usage extension allows use of the public key
      // for
      // verifying signatures on oter certificates.

      // ---BEGIN STUDENT CODE---
      throw new UnsupportedOperationException("Please implement this operation.");
      // ---END STUDENT CODE---
    }
  },

  /**
   * Public key in the certificate is to be used for generating or verifying
   * digital signatures.
   */
  SIGNATURE {
    @Override
    public boolean isAllowedBy(BasicConstraints basic, KeyUsage usage) {
      // TODO: Check that the public key in the certificate can be used
      // for verifying digital signatures.
      // ---BEGIN STUDENT CODE---
      throw new UnsupportedOperationException("Please implement this operation.");
      // ---END STUDENT CODE---
    }
  },

  /**
   * Public key in the certificate is intended to be used for key encipherment.
   */
  WRAP_KEY {
    @Override
    public boolean isAllowedBy(BasicConstraints basic, KeyUsage usage) {
      // TODO: Check that the public key in the certificate can be
      // used to transport symmetric session keys. (key encipherment)
      // ---BEGIN STUDENT CODE---
      throw new UnsupportedOperationException("Please implement this operation.");
      // ---END STUDENT CODE---
    }
  };

  /**
   * Tests if the intended key operation is allowed by the given combination of
   * X.509 certificate restrictions.
   * 
   * Enumeration members must override this method to provide their own policy
   * implementation.
   * 
   * @param basic
   *          Basic constraints of the certificate.
   * 
   * @param usage
   *          Key usage extensions of the certificate.
   * 
   * @return True if the policies given in the certificate allow the intended
   *         key usage, false otherwise.
   */
  public abstract boolean isAllowedBy(BasicConstraints basic, KeyUsage usage);
}
