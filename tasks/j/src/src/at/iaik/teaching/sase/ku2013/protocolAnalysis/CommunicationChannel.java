package at.iaik.teaching.sase.ku2013.protocolAnalysis;

/**
 * Model of the netwrok channel between the server and client components of a
 * certificate revocation-check blackbox.
 * 
 * This interface models the (insecure) network channel between the server and
 * the client components of a certificate revocation-check blackbox.
 */
public interface CommunicationChannel {
  /**
   * Transmits a requests to the given server and returns the response.
   * 
   * @param server
   *          The target server receiving the request.
   * 
   * @param request
   *          The request byte array to be sent over the channel.
   * 
   * @return The response byte array received from the channel.
   * 
   * @throws ResourceCheckException
   *           may be thrown by the underlying channel or server implementation,
   *           in case of unexpected error conditions.
   */
  byte[] transmit(ResourceCheckServer server, byte[] request)
      throws ResourceCheckException;
}
