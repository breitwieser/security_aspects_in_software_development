<%@page import="at.tugraz.iaik.teaching.sase2013.AppResource"%>
<%@ page language="java" contentType="text/html; charset=UTF-8"
	pageEncoding="UTF-8" isErrorPage="true"%>
<%@ taglib prefix="sss" tagdir="/WEB-INF/tags"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<sss:html_head
	title="Sase Submission System - Fuzzy has eaten server logic" />
<body>
	<div id="container">
		<sss:header />
		<div id="lbody" class="lbody">
			<div class="generic_container">
				<h1 class="heading">Fuzzy has eaten this web sites ability to
					understand your request</h1>
				<p class="description description_yellow">
					Fuzzy has foiled your attempt to access
					<code>${pageContext.errorData.requestURI }</code>
					, because
					<code><%=request.getAttribute("javax.servlet.error.message")==null?" of an unknown reason":request.getAttribute("javax.servlet.error.message")==null%></code>
					.
				</p>
				<div class="error_picture">
					<img
						src="${pageContext.request.contextPath}/icons/fuzzy_attacks.png"
						alt="You shall not pass!" width="200" height="155" />
				</div>
				<div class="error_description">
					<p class="error_description">
						You respectfully ask the server for the
						<code>${pageContext.errorData.requestURI }</code>
						resource. Sadly, parts of the server have been eaten by <b
							class="fuzzy_the_bug">Fuzzy the Bug</b>, and the server is
						therefore no longer able to process your request.
					</p>
					<p class="error_description">
						In other words, the server experienced an internal error and
						cannot process your request. You can return to the index page <a
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