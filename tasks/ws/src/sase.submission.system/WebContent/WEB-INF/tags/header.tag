<%@tag import="at.tugraz.iaik.teaching.sase2013.AppResource"%>
<%@tag import="at.tugraz.iaik.teaching.sase2013.ErrorCommunique"%>
<%@tag import="at.tugraz.iaik.teaching.sase2013.Utils"%>
<%@ tag language="java" pageEncoding="UTF-8"%>

<div id="header">
	<div id="fuzzy_the_bug_div">
		<a href="<%=AppResource.ROOT_INDEX.getFullPath()%>"> <img
			id="fuzzy_the_bug"
			src="${pageContext.request.contextPath}/icons/fuzzy_medium.png"
			alt="Fuzzy the Bug" /></a>
	</div>
	<div id="title_div" class="title_text">
		<span id="title">SASE SUB SYS</span>
	</div>
	<div id="session_mngt_div" class="title_text">
		<%
			if (Utils.getUser(request) != null) {
		%>
		<a id="header_button" class="mod_button mod_button_yellow"
			href="<%=AppResource.LOGOUT.getFullPath()%>"> <img class="button_icon"
			src="${pageContext.request.contextPath}/icons/user.png" /> Log out
			...
		</a>
		<%
			} else if (!request.getServletPath().equals(AppResource.SIGNIN.getPath())) {
		%>
		<a id="header_button" class="mod_button mod_button_yellow"
			href="${pageContext.request.contextPath}/signin.jsp"> <img
			class="button_icon"
			src="${pageContext.request.contextPath}/icons/user.png" /> Sign in
			...
		</a>
		<%
			}
		%>
	</div>
</div>
<div class="kill_float"></div>
<div id="ComAreaDataStore" class="hidden">
	<%
		ErrorCommunique e = ErrorCommunique.getCommunique(session);
		if (e != null) {
			e.toJson(out);
			ErrorCommunique.clearCommunique(session);
			//TODO getStatus Code and Request URI
		}
	%>
</div>
<div id="CommArea" class="comm_area">
	<div id="CommMessage" class="comm_message">Message</div>
	<div id="CommClose" class="comm_close round_button mod_button_grey">x</div>
</div>
<div id="MessageOverlay"></div>
