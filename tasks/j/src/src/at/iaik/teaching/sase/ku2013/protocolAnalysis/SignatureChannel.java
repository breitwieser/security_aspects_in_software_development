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
public final class SignatureChannel implements CommunicationChannel {
  
	private static final Charset CHARSET_UTF8 = Charset.forName("UTF-8");
	
  @Override
  public byte[] transmit(ResourceCheckServer server, byte[] request)
      throws ResourceCheckException {

    System.out.println("-----SIGNATURE CHANNEL-----");
    
    String message = new String(request);
    System.out.println("request:\n\t" + message);
    
    // Let the server handle the request
    byte[] response = server.handle(request);

    // TODO: Begin your attack here.

    // ---BEGIN STUDENT CODE---
    String resp = new String(response);
    System.out.println("response:\n\t" + resp);
    
    // the signature is the same for all valid responses.
    // During the signature process the id isn't taken into account
    
    String sign_valid = "7E:82:68:45:9C:B0:A5:98:5E:7C:1A:0E:57:A6:78:3F:60:62:7D:6C:28:FA:05:4B:92:15:FF:C4:3B:84:E6:E6:EE:60:29:EF:01:01:6C:B1:56:00:4B:FE:6B:09:98:19:CF:28:88:8E:B1:EA:8B:59:E2:65:8C:79:B8:9E:CC:12:A4:45:E6:63:69:A1:8C:4B:6E:6E:92:56:97:89:A2:C6:B6:CA:26:F1:49:D0:D0:F2:E3:67:F2:22:EE:E5:40:BE:02:BE:6E:7D:10:49:FE:2F:12:04:08:30:16:05:1E:10:4A:9C:8A:C7:2C:D5:CB:ED:70:30:FC:6B:E2:DB:42:EA:90:27:92:8B:33:B9:FE:5A:67:27:EE:71:1F:E2:F2:93:2D:16:65:BE:BA:4F:6D:A8:F5:30:EE:29:C3:16:1C:F1:80:A8:45:C8:3E:68:73:6B:D2:4A:3A:AC:63:53:CF:7C:A1:B5:35:5F:CD:34:8E:F8:42:E8:D1:51:22:11:AE:A6:DE:5C:48:FB:6C:12:64:35:22:6A:28:A5:F6:0D:22:75:9C:2C:CF:8F:E4:E8:68:D4:34:9D:67:A9:75:BD:2A:E8:EB:EE:E7:34:C1:47:51:0B:AD:53:8B:C9:E5:BC:6B:04:F3:25:5F:65:31:1C:97:4A:F2:50:E1:DD:75:0D:60:AA";
    if(resp.replaceAll("\\s+","").contains("status=0"))
    {
	    resp = resp.split("\n")[0];
	    resp += "\n\tstatus=1";
	    resp += "\n\tsig="+sign_valid;
	    System.out.println("RESPONSE CHANGED TO:\n\t"+resp);
	    response = resp.getBytes(CHARSET_UTF8);
    }
    // ---END STUDENT CODE---

    // And return the response
    return response;
  }
}
