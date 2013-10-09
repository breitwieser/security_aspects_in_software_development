<%@page import="at.tugraz.iaik.teaching.sase2013.AppResource"%>
<%@ page language="java" contentType="text/html; charset=UTF-8"
	pageEncoding="UTF-8" isErrorPage="true"%>
<%@ taglib prefix="sss" tagdir="/WEB-INF/tags"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<sss:html_head
	title="Sase Submission System - Fuzzy is barring entry to this resource" />
<body>
	<div id="container">
		<sss:header />
		<div id="lbody" class="lbody">
			<div class="generic_container">
				<h1 class="heading">Fuzzy is preventing you from accessing this
					resource</h1>
				<p class="description description_yellow">
					Fuzzy has foiled your attempt to access
					<code>${pageContext.errorData.requestURI }</code>
					.
				</p>
				<div class="error_picture">
					<img
						src="${pageContext.request.contextPath}/icons/fuzzy_attacks.png"
						alt="You shall not pass!" width="200" height="155" />
				</div>
				<div class="error_description">
					<p class="error_description">
						You stealthily make your way along the path.
						<code>${pageContext.errorData.requestURI }</code>
						cannot be far ahead. Suddenly an enormous shadow looms over the
						horizon. It is <b class="fuzzy_the_bug">Fuzzy the Bug</b>. Panic
						stricken by its ravenous appearance you decide that discretion is
						the better part of valor and beat a hasty retreat.
					</p>
					<p class="error_description">
						In other words, you are not authorized to access this resource.
						You can return to the index page <a class="error_link"
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