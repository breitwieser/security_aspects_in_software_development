<%@page import="at.tugraz.iaik.teaching.sase2013.AppResource"%>
<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<%@ taglib prefix="sss" tagdir="/WEB-INF/tags" %>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<sss:html_head title="Sase Submission System Login"/>
<body>
<div id="container">
  <sss:header />
<div id="lbody" class="lbody">
<div id="signin_form_div" class="form_container form_container_yellow">
<form id="signin_form" method="POST" action="<%=AppResource.SIGNIN_DO.getFullPath() %>" >
  <h1 class="heading">Sign in</h1>
  <p class="description description_yellow">
  	Enter your data to register with the SASE Submission System
  </p>  
  <div class="spacer"></div>
  <label for="name" class="form_label signin_form_label">Name</label>
  <input id="name" type="text" name="name" class="form_input signin_form_input " tabindex="1" />
  <div id="name_error_desc" class="form_error_description signin_form_error_description"></div>
  
  <div class="spacer"></div>  
  <label for="plainpass" class="form_label signin_form_label">Password</label>
  <input id="plainpass" type="password" name="plainpass" class="form_input signin_form_input" tabindex="2" />
  <div id="pass_error_desc" class="form_error_description signin_form_error_description"></div>
  <input id="pass" type="hidden" name="pass" />

	<div class="spacer"></div>
	<label for="email" class="form_label signin_form_label">Email</label>
  <input id="email" type="text" name="email" class="form_input signin_form_input" tabindex="3" />
  <div id="email_error_desc" class="form_error_description signin_form_error_description"></div>
  
  <div class="spacer"></div>
	<label for="group" class="form_label signin_form_label">Group number</label>
  <input id="group" type="text" name="group" class="form_input signin_form_input" tabindex="4" />
  <div id="goup_error_desc" class="form_error_description signin_form_error_description"></div>
  <sss:iter /><sss:salt />
  <div class="spacer"></div>
  <div id="signin_button_spacer" class="form_label signin_form_label"></div>
  <div id="signin_form_button"class="form_button login_form_button">
    <a id="signin_form_submit_button" class="mod_button mod_button_black" tabindex="5">
    	Sign in!
    </a>
  </div>
  <div class="spacer"></div>
</form>
</div>
</div>
<sss:footer />
</div>
</body>
</html>