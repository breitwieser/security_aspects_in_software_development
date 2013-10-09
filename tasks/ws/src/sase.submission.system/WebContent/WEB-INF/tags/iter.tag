<%@tag import="at.tugraz.iaik.teaching.sase2013.SaseSubmissionSystem"%>
<%@tag import="java.io.IOException"%>
<%@tag import="at.tugraz.iaik.teaching.sase2013.db.GetPwdIterSpec"%>
<%@tag import="at.tugraz.iaik.teaching.sase2013.db.ResultDatabaseOperation"%>
<%@tag import="at.tugraz.iaik.teaching.sase2013.Utils"%>
<%@tag import="at.tugraz.iaik.teaching.sase2013.db.DatabaseOperationExecutor"%>
<%@ tag language="java" pageEncoding="UTF-8"%>
<%!
private int getIter(String iterParam) {
	DatabaseOperationExecutor exec = DatabaseOperationExecutor.getInstance();
	int iter = -1;
	if (!Utils.isStringEmpty(iterParam)) {
		try {
			iter = Integer.parseInt(iterParam.trim());
		} catch (NumberFormatException e) {}	
	}
	if (iter == -1) {
		ResultDatabaseOperation rop = new GetPwdIterSpec();
		try {
			exec.executeDbOperation(rop);
		} catch (ServletException e) {
			iter = SaseSubmissionSystem.DEFAULT_PWD_DIGEST_ITERATIONS;
		} catch (IOException e) {
			iter = SaseSubmissionSystem.DEFAULT_PWD_DIGEST_ITERATIONS;
		}
		iter = (Integer)rop.getResult();
	}
	return iter;
}
%>

<input id="iter" type="hidden" name="iter" value="<%=getIter(request.getParameter("iter"))%>" />
