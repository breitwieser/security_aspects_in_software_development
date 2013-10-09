<%@page import="at.tugraz.iaik.teaching.sase2013.AppResource"%>
<%@ page language="java" contentType="text/html; charset=UTF-8"
	pageEncoding="UTF-8"%>
<%@ taglib prefix="sss" tagdir="/WEB-INF/tags"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<sss:html_head title="Sase Submission System Program Submission" />
<body>
	<div id="container">
		<sss:header />
		<div id="lbody" class="lbody">
			<div class="form_container form_container_yellow">
				<h1 class="heading">Program Submission</h1>
				<p class="description description_yellow">
					Here you can submit your program. For your program to be suitable
					for submission it must consist of exactly two source files:
					'main.c' and 'submission.c'. Additional header files are
					acceptable. To submit your program compress all required header and
					source files into a single zip file and submit this zip file. The
					zip file will be uncompressed on the server and compiled using this
					command	<sss:cc />.
				</p>
				<form id="program_submission" method="POST"
					enctype="multipart/form-data"
					action="<%=AppResource.SUBMISSION_DO_UPLOAD.getFullPath()%>">

					<div id="file_choice_button" class="form_button submission_form_button">
						<a id="file_choice_select_button" class="mod_button mod_button_black" tabindex="1">Select program</a>
					</div>
					<div id="file_choice_description" class="form_input submission_form_input"></div>
					<div id="file_error_desc" class="form_error_description submission_form_error_description"></div>
					
 					<input id="file_select_hidden" name="file_select_hidden" type="file"
						accept="application/zip" style="display: none;" />

					<div class="spacer"></div>
					<!-- <div id="signin_button_spacer" class="form_label signin_form_label"></div> -->
					<div id="file_form_button" class="form_button submission_form_button">
						<a id="file_form_submit_button"
							class="mod_button mod_button_black" tabindex="2"> Submit! </a>
					</div>
					<div class="spacer"></div>
				</form>
				<div id="program_report"></div>
			</div>
		</div>
		<sss:footer />
	</div>
</body>
</html>