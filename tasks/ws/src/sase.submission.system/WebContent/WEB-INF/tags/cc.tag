<%@tag import="at.tugraz.iaik.teaching.sase2013.db.ResultDatabaseOperation"%>
<%@tag import="at.tugraz.iaik.teaching.sase2013.db.GetCompilerCommand"%>
<%@tag import="java.io.IOException"%>
<%@tag import="at.tugraz.iaik.teaching.sase2013.db.DatabaseOperationExecutor"%>
<%@ tag language="java" pageEncoding="UTF-8"%>
<%!
private String getCompilerCommand() throws IOException, ServletException {
	DatabaseOperationExecutor exec = DatabaseOperationExecutor.getInstance();
	ResultDatabaseOperation rop = new GetCompilerCommand();
	exec.executeDbOperation(rop);
	return ((String)rop.getResult()).trim();
}
%>
<code>'<%=getCompilerCommand()%>'</code>
