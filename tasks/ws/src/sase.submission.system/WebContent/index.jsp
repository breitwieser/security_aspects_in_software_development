<%@ page language="java" contentType="text/html; charset=UTF-8"
	pageEncoding="UTF-8"%>
<%@ taglib prefix="sss" tagdir="/WEB-INF/tags"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<sss:html_head title="Welcome to the SASE Submission System" />
<body>
	<div id="container">
		<sss:header />
		<div id="lbody" class="lbody">
			<sss:menu />
			<div class=next_to_menu_container style="">
				<h1 class="next_to_menu">Welcome to the SASE Submission System</h1>
				<p class="description description_yellow">The SASE Submission
					System (SASE SUB SYS) is the basis for the web security assignment
					of the Security Aspects in Software Development (Sicherheitsaspekte
					in der Softwareentwicklung (SASE)) practical exercises in the
					winter semester 2013/2014 (Course number: 705.025, Graz University
					of Technology).</p>
				<div>
					<h2 class="heading">About</h2>
					<p>
						The SASE SUB SYSTEM is a simple web application that allows
						students that is you to upload C source code and compile it on the
						server. Before uploading you must register. Registering requires a
						user name, password, email address and group number. Once
						registered you can use the <i>Student Area</i> of the web
						application to upload and compile your C source.
					</p>
					<p>
						The system also provides an administration interface in the <i>Admin
							Area</i> where you can delete users or change the SASE SUB SYSTEM's
						configuration, including which compiler command to use. The
						default credentials for the Admin Area are
					</p>
					<ul>
						<li>User name: admin</li>
						<li>Password: sase2013
					</ul>
					<h2 class="heading">Disclaimer</h2>
					<p>This system was developed to train students to find and fix
						security issues in web applications. As such it contains a number
						of intentional vulnerabilities that can be used to attack this web
						application. The list of intentional security vulnerabilities
						includes, but is not limited to:</p>
					<ul>
						<li>A file upload vulnerability that allows uploading
							arbitrary files to all location accessible to the web server
							user.</li>
						<li>A command injection vulnerability that allows executing
							arbitrary commands on server, running with the privileges of the
							web server user.</li>
						<li>A sequel injection that allows reading arbitrary data
							from the SASE SUB SYSTEM database</li>
					</ul>
					<p>If you choose to use the SASE SUB SYSTEM as a whole, or
						parts of it, make sure that you are aware of any security
						vulnerabilities contained therein.</p>
					<p>Don't feed the Bug!</p>
				</div>
			</div>
		</div>
		<sss:footer />
	</div>
</body>
</html>