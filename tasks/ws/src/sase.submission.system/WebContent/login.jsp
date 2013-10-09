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

<div id="login_form_div" class="form_container form_container_yellow">
<form id="login_form" method="POST" action="j_security_check" >
  <h1 class="heading">Log in</h1>
  <p class="description description_yellow">
    Enter your credentials to log in to the SASE Submission System
  </p> 
  <div class="spacer"></div>
  <label for="j_username" class="form_label login_form_label">Name</label>
  <input id="j_username" type="text" name="j_username" class="form_input login_form_input" tabindex="1" />
  <div id="name_error_desc" class="form_error_description login_form_error_description"></div>

  <div class="spacer"></div>  
  <label for="password" class="form_label login_form_label">Password</label>
  <input id="password" type="password" name="password" class="form_input login_form_input" tabindex="2" />
  <div id="pwd_error_desc" class="form_error_description login_form_error_description"></div>
  <input id="j_password" type="hidden" name="j_password" />
  
  <div class="spacer"></div>
  <div id="login_button_spacer" class="form_label login_form_label"></div>
  <div id="login_form_button" class="form_button login_form_button">
    <a id="login_form_submit_button" class="mod_button mod_button_black" tabindex="3">
    Log in!
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