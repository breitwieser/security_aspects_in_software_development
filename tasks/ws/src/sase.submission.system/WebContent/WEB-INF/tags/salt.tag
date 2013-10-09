<%@tag import="java.util.Random"%>
<%@ tag language="java" pageEncoding="UTF-8"%>
<%!

Random rand = new Random((long)"B@DS33D".hashCode() * 42L);

private String getSalt(String saltParam) {
	StringBuffer saltBuffer = new StringBuffer(32);
	if (saltParam == null) {
		byte[] salt = new byte[16];
		rand.nextBytes(salt);
		for (byte b : salt) {
			saltBuffer.append(String.format("%02x", b & 0xff));
		}
	} else {
		saltBuffer.append(saltParam.trim());
	}
	return saltBuffer.toString();
}
%>
<input id="salt" type="hidden" name="salt" value="<%=getSalt(request.getParameter("salt")) %>" />