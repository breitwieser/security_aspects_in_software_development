<%@tag import="java.security.SecureRandom"%>
<%@tag import="java.util.Random"%>
<%@ tag language="java" pageEncoding="UTF-8"%>
<%!
//FIX 1g Fixed salt vulnerability
//Random rand = new Random((long)"B@DS33D".hashCode() * 42L);
SecureRandom rand = new SecureRandom();

private String getSalt(String saltParam) {
	//FIX 1f: 
	StringBuffer saltBuffer = new StringBuffer(32);
// 	if (saltParam == null) {
		byte[] salt = new byte[16];
		rand.nextBytes(salt);
		for (byte b : salt) {
			saltBuffer.append(String.format("%02x", b & 0xff));
		}
// 	} else {
// 		saltBuffer.append(saltParam.trim());
// 	}
	return saltBuffer.toString();
}
%>
<input id="salt" type="hidden" name="salt" value="<%=getSalt(request.getParameter("salt")) %>" />