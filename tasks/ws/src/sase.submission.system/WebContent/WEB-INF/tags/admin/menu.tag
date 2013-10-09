<%@tag import="at.tugraz.iaik.teaching.sase2013.AppResource"%>
<%@tag import="at.tugraz.iaik.teaching.sase2013.MenuEntry"%>
<%@ tag language="java" pageEncoding="UTF-8"%>
<%!public static final MenuEntry[] ENTRIES = {
	    new MenuEntry("Configuration", AppResource.ADMIN_CONFIG),
	    new MenuEntry("Users", AppResource.ADMIN_USERS) };%>

<div id="menu" class="menu">
<% MenuEntry.printHtmlMenu(out, ENTRIES); %>
</div>