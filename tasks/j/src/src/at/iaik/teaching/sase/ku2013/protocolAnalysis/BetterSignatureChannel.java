package at.iaik.teaching.sase.ku2013.protocolAnalysis;

import java.nio.charset.Charset;
import java.util.HashMap;

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
public final class BetterSignatureChannel implements CommunicationChannel {

  private static final Charset CHARSET_UTF8 = Charset.forName("UTF-8");
	  
	@Override
  public byte[] transmit(ResourceCheckServer server, byte[] request)
      throws ResourceCheckException {

    System.out.println("-----BETTER SIGNATURE CHANNEL-----");
    
    String message = new String(request);
    System.out.println("request:\n\t" + message);
    
    // Let the server handle the request
    byte[] response = server.handle(request);

    // TODO: Begin your attack here.

    // ---BEGIN STUDENT CODE---
    String resp = new String(response);
    System.out.println("response:\n\t" + resp);
    
    // this simulates a replay attack
    // the attacker stores all responses for requests with a valid response
    // once the certificate is revoked, he replays the recorded response
    
    // represents the recorded database with certificate id and valid_response_signature
    HashMap<String, String> response_db = new HashMap<String, String>();
    response_db.put("83:6B:E8:78:32:8D:45:68:19:C6:1F:5A:3F:E5:56:2A:13:B6:86:C7", "22:8A:3D:AB:EC:88:3E:30:ED:46:76:20:7B:91:1F:D2:0B:90:D0:76:D0:04:82:0A:AB:65:EA:59:67:53:A0:3A:2F:9C:4E:AA:68:CB:0F:85:24:D8:32:27:B7:60:98:3A:FC:97:54:A3:70:A3:FF:07:6C:1D:D9:D2:CC:62:74:29:D0:B4:E9:1A:A9:01:F5:E0:47:C4:84:ED:96:43:3A:03:80:AA:00:EB:79:30:8E:ED:EB:EC:3E:DE:22:5E:D3:6A:4C:D8:7F:18:8A:37:4B:18:6E:0A:DA:08:49:AD:D9:CF:72:D8:FB:4E:D5:74:96:C8:20:11:C5:9B:89:40:53:9C:03:1F:9F:AF:47:6D:53:33:70:62:1B:28:6A:78:E3:45:E6:97:F4:0D:33:E9:4C:F6:3F:66:6C:E6:E5:77:C8:C7:F2:A2:FF:EA:F9:89:C2:91:DD:E2:12:C2:2F:7E:35:0F:B7:3A:6B:A1:DE:61:BE:B4:D7:C3:4A:61:8B:5C:AD:78:2F:E6:87:5A:44:88:27:EE:44:4D:4D:BD:F0:1B:D8:D1:D9:06:FD:C2:36:94:A0:24:69:8D:F2:29:9C:60:81:A2:36:48:1D:E1:55:0B:8E:48:9B:17:48:EC:E7:05:F0:6F:D2:82:75:BE:A7:2A:A0:38:66:9F:AB:2B:5F:78:92:9B");
    response_db.put("BC:51:62:24:53:DD:1A:6C:CC:59:95:60:E5:AE:06:59:B5:E1:19:11", "BE:2D:FF:AD:FF:16:AD:1C:9F:64:6E:44:78:1D:4F:63:D9:E1:2F:82:10:98:75:7E:76:22:27:8F:87:BA:D5:D0:96:A8:59:A9:36:D6:00:B3:F1:3F:AC:46:41:52:E0:FD:4D:E4:72:FD:FF:21:C9:0D:58:69:94:6B:3A:1C:C1:79:36:A1:46:22:08:DD:BA:0F:97:7D:98:E6:74:18:B4:8B:AB:5A:4F:19:10:B2:D5:24:ED:0B:6E:A6:D7:20:DF:47:3E:A0:C1:77:BE:FC:AF:82:72:1E:F7:1A:27:34:E7:AF:34:51:48:5B:CB:03:6A:BE:D9:CF:70:5D:F5:A7:23:24:B5:05:B7:05:E6:96:38:C2:0A:F8:ED:99:37:52:BD:49:2D:FE:CC:AB:50:0B:EA:10:F6:A8:3E:EB:C9:B1:79:D5:E5:B2:1C:BF:0D:2F:B0:22:34:0D:EE:1E:50:C5:1B:65:93:E1:1E:50:CA:25:EA:85:2F:77:72:BF:44:A7:9A:63:3A:9C:97:C5:80:CC:BF:82:34:D4:A8:B1:30:ED:2E:65:0B:D5:EF:16:AE:F2:98:7F:5B:D5:5B:0F:F5:89:8D:30:9B:6E:75:DE:CD:2B:E4:51:2B:4C:1D:1A:FA:9B:4D:42:A9:35:CE:0B:25:1D:A3:8A:A1:D0:DB:34:03:A8:09:39");
    response_db.put("5B:C6:A2:02:64:3B:8E:F8:BA:94:5A:6E:56:21:16:A5:13:F2:40:99", "4C:5C:D4:1E:9B:47:7D:ED:47:FF:07:AE:CD:D1:7F:45:DA:26:54:E7:AE:92:5F:81:6A:86:B8:94:09:6F:BC:C0:94:BC:2E:EB:3E:33:19:EE:93:3C:83:1D:6C:20:AF:FB:CA:28:C1:62:A2:58:08:21:C8:54:5D:93:1D:EE:43:F1:A2:76:A1:DC:C5:18:E5:5B:9E:A6:50:49:F1:32:3F:B5:63:EB:2A:BF:95:76:0F:D4:E9:D0:24:4B:35:BA:A9:F4:F2:E9:75:15:9C:EB:43:27:33:35:E7:2D:96:E5:01:50:E4:4B:5F:DE:07:F7:3B:46:69:D1:B5:AC:C6:F5:B3:5E:41:20:89:A8:1E:B2:66:27:A7:DE:E3:0A:50:19:B8:1E:C9:DF:6F:11:F1:94:6E:7A:C1:16:81:D6:21:B2:76:33:A1:BF:4B:6D:5C:33:EA:39:D6:18:1C:22:EF:FF:0D:39:82:73:02:EC:FC:3E:BA:28:CD:1D:29:2B:03:1A:C1:C6:F6:2F:B9:A3:44:FE:41:AD:82:D6:36:FE:27:FF:89:43:94:59:91:20:17:0F:71:A6:79:0E:B0:3A:96:76:8A:EE:B2:DC:08:05:BE:62:64:F0:5A:92:6D:A9:0F:0F:1D:54:BB:5C:43:00:9A:1E:91:A3:0C:B6:8C:79:D8:38:B6:10");
    response_db.put("A6:34:D0:58:87:6A:AE:2D:54:DB:42:36:81:74:FE:83:36:40:AE:29", "02:35:51:EE:A9:38:87:BA:D5:2D:83:E9:68:E6:AC:2F:12:AD:83:4A:0E:46:78:26:64:2E:D3:29:91:1D:85:EC:76:43:CF:D2:D7:40:95:6B:D0:99:37:35:17:DB:A9:92:DA:16:E5:ED:9E:25:47:DE:E9:60:F3:39:20:C5:CA:26:92:80:99:2B:7A:30:81:83:FB:69:3D:06:1A:99:C5:99:91:0C:0B:BA:AE:28:B3:23:8A:20:E0:1A:8E:C2:E9:0F:D1:A6:2E:52:4D:72:49:3C:47:3F:A1:09:32:F3:82:B9:B0:56:39:D5:6E:5A:CC:DB:4B:D4:2C:C4:F4:09:35:F6:09:8D:AD:0A:DE:34:AE:F4:C0:FF:CE:08:92:75:7E:27:4B:28:6F:58:89:FE:ED:6A:2A:1E:B5:6D:AE:B6:B9:C4:71:EC:0A:45:8C:EB:B3:7D:61:B7:26:45:91:D1:2A:38:B9:C1:72:FE:4F:B2:22:6D:79:84:49:1E:E4:A3:8F:81:45:3F:81:F3:40:F2:E5:06:90:9A:B7:F6:6F:0E:1D:F3:56:05:70:23:89:1D:5F:D5:46:25:8B:53:CF:B5:D5:BC:C3:74:46:C7:82:28:A4:93:97:8D:AE:D5:7A:BF:C8:26:50:3A:BC:D0:3E:29:3F:AA:17:95:73:2E:F4:86:3F:E3");
    response_db.put("1C:52:88:CD:74:E8:4C:97:18:A0:91:38:4F:AA:79:80:51:D6:6F:31", "38:5A:64:B6:A0:4B:2B:37:4D:EC:E7:7D:B3:43:0D:0C:9E:DD:92:1D:9A:93:C5:0E:5C:AE:42:50:A1:05:DD:25:83:3E:72:C4:F8:BC:6B:D3:78:97:E3:71:9D:FD:1D:0C:84:A2:71:FF:5D:77:20:DF:65:F6:0B:E2:6F:E3:1F:0F:68:1E:05:3A:A5:A5:0F:AA:BF:9D:06:9F:C6:10:58:72:EB:F3:0F:52:3A:15:AD:E4:CC:AE:9E:8D:9C:D3:C3:7E:CF:83:D3:EC:BB:3A:91:1C:96:73:1A:D3:BA:C0:43:DA:F4:1F:00:C0:AE:1A:DD:7A:EF:AC:4F:CE:C3:85:34:0A:9B:77:15:44:F3:FF:DA:07:D1:0F:C6:C7:F9:0B:EF:AC:F7:3F:EC:4B:5A:3F:D4:78:0C:8C:11:60:5F:1E:38:CB:04:F0:B7:81:BE:5F:A4:E3:5C:22:13:3B:29:C2:0C:12:8C:14:BB:40:2E:F4:96:92:86:13:49:D6:B2:BF:F8:26:05:AF:0D:8A:3D:20:B6:F3:A9:FD:07:DB:B0:8D:CC:94:5A:55:AB:01:CB:E9:1A:5B:93:75:7C:1F:E6:91:E6:ED:6D:80:F3:E7:22:8A:BA:44:92:97:19:81:6D:2A:97:8F:F2:4C:1A:85:8C:13:16:A7:87:EE:14:63:5D:2E:15:87");
    
    String first_line = resp.split("\n")[0];
    String id = first_line.replaceAll("\\s+","").split("=")[1];

    if(resp.replaceAll("\\s+","").contains("status=0") && response_db.containsKey(id))
    {
	    resp = resp.split("\n")[0];
	    resp += "\n\tstatus=1";
	    resp += "\n\tsig="+response_db.get(id);
	    System.out.println("RESPONSE CHANGED TO:\n\t"+resp);
	    response = resp.getBytes(CHARSET_UTF8);
    }
    // ---END STUDENT CODE---

    // And return the response
    return response;
  }
}
