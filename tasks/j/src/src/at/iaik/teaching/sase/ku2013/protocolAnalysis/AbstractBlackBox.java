package at.iaik.teaching.sase.ku2013.protocolAnalysis;

import iaik.x509.X509Certificate;

/**
 * Abstract base-class implementation for revocation check "black-boxes".
 */
public abstract class AbstractBlackBox implements ResourceChecker {
  /**
   * The communication channel used by this black box.
   */
  protected final CommunicationChannel channel;

  /**
   * Constructs a new black box.
   * 
   * @param database
   *          The revocation check database to be used by this black-box. The
   *          base class implementation of this constructor only asserts, that
   *          this object is not null.
   * 
   * @param channel
   *          The communication channel to be used by this black-box, for
   *          communication between the server and client components.
   */
  public AbstractBlackBox(ResourceDatabase database,
      CommunicationChannel channel) {

    if (database == null) {
      throw new IllegalArgumentException(
          "Revocation check database must not be null.");
    }

    if (channel == null) {
      throw new IllegalArgumentException(
          "Communication channerl must not be null.");
    }

    this.channel = channel;
  }

  /*
   * (non-Javadoc)
   * 
   * @see at.iaik.teaching.sase.ku2013.revocation.RevocationChecker#check(iaik.
   * x509.X509Certificate)
   * 
   * The implementation of the check method is provided by the actual black-box
   * implementation. It will use the channel to communicate requests and its
   * black-box specific server implementation to do the checking.
   */
  @Override
  public abstract ResourceCheckResult check(X509Certificate certificate)
      throws ResourceCheckException;
}
