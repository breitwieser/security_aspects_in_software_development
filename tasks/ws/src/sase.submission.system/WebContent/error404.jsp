<%@page import="at.tugraz.iaik.teaching.sase2013.AppResource"%>
<%@ page language="java" contentType="text/html; charset=UTF-8"
	pageEncoding="UTF-8" isErrorPage="true"%>
<%@ taglib prefix="sss" tagdir="/WEB-INF/tags"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<sss:html_head
	title="Sase Submission System - Fuzzy has eaten this resource" />
<body>
	<div id="container">
		<sss:header />
		<div id="lbody" class="lbody">
			<div class="generic_container">
				<h1 class="heading">Fuzzy has eaten this resource</h1>
				<p class="description description_yellow">
					You have tried to access
					<code>${pageContext.errorData.requestURI }</code>
					, but there is simple nothing there any more.
				</p>
				<div class="error_picture">
					<img
						src="${pageContext.request.contextPath}/icons/fuzzy_attacks.png"
						alt="You shall not pass!" width="200" height="155" />
				</div>
				<div class="error_description">
					<p class="error_description">
						You have followed the path to
						<code>${pageContext.errorData.requestURI }</code>
						, but as you arrive you simply find nothing. Not a trace, not even
						a big black hole in the ground; just nothing. It is as if
						<code>${pageContext.errorData.requestURI }</code>
						has never existed at all. On the horizon you make out the
						retreating form of <b class="fuzzy_the_bug">Fuzzy the Bug</b>. You
						are not quite sure, as such things are difficult to tell for
						humongous, yellow bugs, but you have feeling that <b
							class="fuzzy_the_bug">Fuzzy</b> is rather content. It seems
						satiated, at least for the time beeing.
					</p>
					<p class="error_description">
						In other words, you have tried to access a resource that simply
						does not exist. You can return to the index page <a
							class="error_link"
							href="<%=AppResource.ROOT_INDEX.getFullPath()%>">here.</a>
					</p>
				</div>
				<div class="kill_float"></div>
			</div>
		</div>
		<sss:footer />
	</div>
</body>
</html>