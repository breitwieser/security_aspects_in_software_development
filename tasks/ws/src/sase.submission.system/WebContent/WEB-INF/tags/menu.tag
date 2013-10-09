<%@tag import="at.tugraz.iaik.teaching.sase2013.AppResource"%>
<%@tag import="at.tugraz.iaik.teaching.sase2013.MenuEntry"%>
<%@ tag language="java" pageEncoding="UTF-8"%>
<%!public static final MenuEntry[] ENTRIES = {
	    new MenuEntry("Student Area", AppResource.SUBMISSION_INDEX),
	    new MenuEntry("Admin Area", AppResource.ADMIN_INDEX) };%>
<div id="menu" class="menu">
<% MenuEntry.printHtmlMenu(out, ENTRIES); %>
</div>