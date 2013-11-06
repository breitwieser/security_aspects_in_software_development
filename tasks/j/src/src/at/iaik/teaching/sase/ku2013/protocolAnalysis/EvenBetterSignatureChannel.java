package at.iaik.teaching.sase.ku2013.protocolAnalysis;

/**
 * Plain channel implementation without any response or request modifications.
 * 
 * This class provides a default implementation of the
 * {@link CommunicationChannel} interface, which forwards requests between a
 * {@link ResourceChecker} and a {@link ResourceCheckServer} without any
 * modifications.
 * 
 * Conceptually this class is a model of a network channel without any active
 * man-in-the-middle.
 */
public final class EvenBetterSignatureChannel implements CommunicationChannel {
  
  @Override
  public byte[] transmit(ResourceCheckServer server, byte[] request)
      throws ResourceCheckException {

    System.out.println("-----EVEN BETTER SIGNATURE CHANNEL-----");
    
    String message = new String(request);
    System.out.println("request:\n\t" + message);
    
    // Let the server handle the request
    byte[] response = server.handle(request);

    // TODO: Begin your attack here.

    // ---BEGIN STUDENT CODE---
    String resp = new String(response);
    System.out.println("response:\n\t" + resp);
    // ---END STUDENT CODE---

    // And return the response
    return response;
  }
}
