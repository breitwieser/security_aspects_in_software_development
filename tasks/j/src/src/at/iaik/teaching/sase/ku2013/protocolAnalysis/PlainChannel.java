package at.iaik.teaching.sase.ku2013.protocolAnalysis;

import java.nio.charset.Charset;

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
public final class PlainChannel implements CommunicationChannel {
  
  private static final Charset CHARSET_UTF8 = Charset.forName("UTF-8");
	
  @Override
  public byte[] transmit(ResourceCheckServer server, byte[] request)
      throws ResourceCheckException {

    System.out.println("-----PLAIN CHANNEL-----");
    
    String message = new String(request);
    System.out.println("request:\n\t" + message);
    
    // Let the server handle the request
    byte[] response = server.handle(request);

    // TODO: Begin your attack here.

    // ---BEGIN STUDENT CODE---
    
    //No mechanism to protect integrity
    
    String resp = new String(response);
    System.out.println("response:\n\t" + resp);
    if(resp.replaceAll("\\s+","").contains("status=0"))
    {
    	resp = resp.split("\n")[0];
	    resp += "\n\tstatus=1";
	    System.out.println("RESPONSE CHANGED TO:\n\t"+resp);
	    response = resp.getBytes(CHARSET_UTF8);
    }
    // ---END STUDENT CODE---

    // And return the response
    return response;
  }
}
