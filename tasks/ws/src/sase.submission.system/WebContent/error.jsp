<%@page import="at.tugraz.iaik.teaching.sase2013.ErrorCommunique"%>
<%@ page language="java" contentType="text/html; charset=UTF-8"
	pageEncoding="UTF-8" isErrorPage="true"%>
<%@ taglib prefix="sss" tagdir="/WEB-INF/tags"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<sss:html_head title="Welcome to the SASE Submission System" />
<body>
	<div id="container">
		<%
			ErrorCommunique e = new ErrorCommunique("Hint: Don't feed the bug!",
			    exception);
			e.registerCommunique(session);
			//out.println(pageContext.getErrorData());
		%>
		<sss:header />
		<div id="lbody" class="lbody">
			<div class="generic_container">
				<h1 class="heading">Fuzzy has struck again!</h1>
				<p class="description description_yellow">You have encountered
					Fuzzy the Bug! We will now switch to adventure mode.</p>
				<div class="adventure_container">
					<div class="adventure_picture">
						<img id="fuzzys_adventure" src="${pageContext.request.contextPath}/icons/fuzzy_attacks.png"
							width="200" height="155" />
					</div>
					<div class="adventure_text_container">
						<p>
							You have encountered the humongous and monstrous<b
								class="fuzzy_the_bug"> Fuzzy the Bug</b>. <b
								class="fuzzy_the_bug"> Fuzzy</b> destroyer of innocent software,
							devourer of hapless users has already eaten this web site. Now he
							turns his attention on you. He opens his maw revealing a
							terrifying array of serrated fangs, and addresses you with a
							deep, raspy voice: "<b><span class="shadow">Fuzzy
									hungry! Fuzzy will E A T!"</span></b>. The raspy, cracking quality in his
							speech alone, is enough to strike primal fear into your heart.
							This is the voice a saber tooth tiger would have used, before
							disemboweling its victims. You could have braved his voice, his
							appearance, but the one thing that will haunt you until the end
							of your days is the unsatiable hunger burning in <b
								class="fuzzy_the_bug">Fuzzy's</b> eyes. Luckily for you, this
							won't be long...
						</p>
						<p>You can see:</p>
						<ul>
							<li><b class="fuzzy_the_bug"> Fuzzy the Bug</b></li>
							<li>The gateway to the Internet</li>
						</ul>
						<p>
							You have the following options: Don't be a fool and flee <b
								class="fuzzy_the_bug"> Fuzzy the Bug</b>. Bravely attack <b
								class="fuzzy_the_bug"> Fuzzy the Bug</b> and perish in the
							venture, or feed the bug. If you don't know what to do, type help.
						</p>

						<form id="adventure_form" action="javascript:void(0);">
							<label for="adventure_parser">What do you do?</label> <input
								class="adventure" id="adventure_parser" name="parser"
								type="text" />
						</form>
						<p id="adventure_text_field"></p>

					</div>
				</div>
				<div class="kill_float"></div>
			</div>
		</div>
		<sss:footer />
	</div>
	<div id="adventure_overlay" class="hidden"></div>

</body>
</html>