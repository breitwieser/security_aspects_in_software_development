<%@ page language="java" contentType="text/html; charset=UTF-8"
	pageEncoding="UTF-8"%>
<%@ taglib prefix="sss" tagdir="/WEB-INF/tags"%>
<%@ taglib prefix="sss-admin" tagdir="/WEB-INF/tags/admin"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<sss:html_head
	title="Welcome to the SASE Submission System Administration Interface" />
<body>
	<div id="container">
		<sss:header />
		<div id="lbody" class="lbody">
			<sss-admin:menu />
			<div class=next_to_menu_container style="">
				<h1 class="next_to_menu">Welcome to the administration area of
					the SASE Submission System</h1>
				<p class="description description_yellow">The administration
					area allows you to administer the SASE SUB SYSTEM.</p>
				<div>
					<h2 class="heading">Configuration</h2>
					<p>The <i>configuration</i> menu allows changing global configuration
						parameters of the SASE SUB SYSTEM. The system only supports two
						configuration parameters. The configuration parameters are the
						default number of iterations to digest user passwords, and command
						used to compile student submissions.</p>
					<h2 class="heading">Users</h2>
					<p>
						The <i>Users</i> menu shows all users registered with the system. Both
						administrators and students are presented. Additionally, the users
						menu allows deletion of users from the system. If you delete the
						administration user, and you want to continue to administer the
						SASE SUB SYSTEM, you have to insert a new one in the database, or
						simply rebuild the database by sourcing
						<code>db-init.sql</code>
						in the mysql client.
					</p>
				</div>
			</div>
		</div>
		<sss:footer />
	</div>
</body>
</html>