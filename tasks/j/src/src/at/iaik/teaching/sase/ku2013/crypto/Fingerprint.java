package at.iaik.teaching.sase.ku2013.crypto;

import iaik.x509.X509Certificate;

import java.security.NoSuchAlgorithmException;
import java.util.HashMap;
import java.util.HashSet;

/**
 * Fingerprint (SHA1) of a certificate.
 * 
 * This class encapsulates functionality to work with X.509 certificate
 * finger-prints. The class implements the required methods to allow use of
 * finger-prints as keys in keyed collections such as {@link HashSet} or
 * {@link HashMap}.
 */
public final class Fingerprint implements Comparable<Fingerprint> {
  /**
   * Hexadecimal digits used by the parser.
   */
  private static final String HEX_DIGITS = "0123456789ABCDEF";

  /**
   * Hash algorithm used by this implementation.
   */
  public static final String HASH_ALGORITHM = "SHA1";

  /**
   * Length of an SHA-1 finger-print.
   */
  public static final int HASH_LENGTH = 20;

  /**
   * Raw message digest.
   */
  private final byte[] digest;

  /**
   * Constructs the finger-print of an X509 certificate from a given certificate
   * object.
   * 
   * @param cert
   *          the certificate to be hashed.
   */
  public Fingerprint(X509Certificate cert) {
    try {
      // Let the IAIK JCE implementation do the heavy lifting
      // (DER encoding + SHA1 hash computation)
      this.digest = cert.getFingerprint(HASH_ALGORITHM);
      if (this.digest.length != HASH_LENGTH) {
        throw new RuntimeException("Unexpected SHA1 digest length (huh?)");
      }

    } catch (NoSuchAlgorithmException e) {
      // Wrap as runtime exception
      throw new RuntimeException("SHA1 hash algorithm is not supported (huh?)",
          e);
    }
  }

  /**
   * Constructs a finger-print object from a given byte array.
   * 
   * @param source
   *          The source byte array for copying the digest.
   * 
   * @param off
   *          Index of the first byte of the digest to copy.
   */
  public Fingerprint(byte[] src, int off) {
    this.digest = CryptoToolBox.dupBytes(src, off, HASH_LENGTH);
  }

  /**
   * Constructs a finger-print object from its string representation.
   * 
   * @param hex_str
   *          String representation of the fingerprint.
   */
  public Fingerprint(String hex_str) {

    // Trim the string and convert to upper-case.
    hex_str = hex_str.toUpperCase().trim();
    if (hex_str.length() != 3 * HASH_LENGTH - 1) {
      throw new RuntimeException("Unexpected SHA1 digest length (huh?)");
    }

    this.digest = new byte[HASH_LENGTH];

    for (int n = 0; n < HASH_LENGTH; ++n) {
      if (n < HASH_LENGTH - 1) {
        // Check for the ':' separator
        if (hex_str.charAt(3 * n + 2) != ':') {
          throw new IllegalArgumentException(
              "Missing separator at byte position " + n);
        }
      }

      int hi = HEX_DIGITS.indexOf(hex_str.charAt(3 * n));
      int lo = HEX_DIGITS.indexOf(hex_str.charAt(3 * n + 1));
      if (hi == -1 || lo == -1) {
        throw new IllegalArgumentException(
            "Illegal characters separator at byte position " + n);
      }

      this.digest[n] = (byte) ((hi << 4) | lo);
    }
  }

  @Override
  public boolean equals(Object obj) {
    if (obj == null || !(obj instanceof Fingerprint)) {
      return false;
    }

    Fingerprint other = (Fingerprint) obj;
    return iaik.utils.CryptoUtils.equalsBlock(this.digest, other.digest);
  }

  @Override
  public int hashCode() {
    return iaik.utils.Util.calculateHashCode(this.digest);
  }

  @Override
  public String toString() {
    return iaik.utils.Util.toString(this.digest);
  }

  @Override
  public int compareTo(Fingerprint other) {
    if (other == null) {
      // treat "null" as less than any non-null object
      return 1;
    }

    // Fast check: Reference equality?
    if (this == other) {
      return 0;
    }

    // Compare length
    int len_this = this.digest.length;
    int len_other = other.digest.length;

    // Compare common prefix bytes
    int len_common = Math.min(len_this, len_other);

    for (int n = 0; n < len_common; ++n) {
      int byte_this = this.digest[n] & 0x00FF;
      int byte_other = other.digest[n] & 0x00FF;

      if (byte_this > byte_other) {
        return 1; // We are larger

      } else if (byte_other > byte_this) {
        return -1; // We are smaller
      }
    }

    if (len_this > len_other) {
      return 1; // Same prefix, we are longer
    } else if (len_other > len_this) {
      return -1; // Same prefix, we are shorter
    }

    // Equal objects
    return 0;
  }

  /**
   * Gets a copy of the message digest held by this fingerprint object.
   * 
   * @return A copy of the fingerprint value.
   */
  public byte[] getDigest() {
    return CryptoToolBox.dupBytes(this.digest, 0, this.digest.length);
  }

  /**
   * Copies this finger-print object to a byte array.
   * 
   * @param dst
   *          Destination byte array.
   * 
   * @param off
   *          Index of the first output byte.
   * */

  public void copyTo(byte[] dst, int off) {
    System.arraycopy(this.digest, 0, dst, off, HASH_LENGTH);
  }
}
