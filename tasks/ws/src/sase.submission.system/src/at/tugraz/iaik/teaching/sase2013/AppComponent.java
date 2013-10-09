/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013;

/**
 * Represents the different application components of the SASE SUB SYSTEM. The
 * SASE SUB SYSTEM web application consists of 3 different components. Each
 * component is distinguished by the request path. The 3 components are:
 * <ul>
 * <li>Root: The root component is the entry point into the web application and
 * provides services such as sign in, log in, log out and the error pages.</li>
 * <li>Submission: The student area where students can submit their programs for
 * compilation.</li>
 * <li>Administration: The administration area for managing the system
 * configuration and users.</li>
 * </ul>
 * 
 * @author SASE 2013 Team
 */
public enum AppComponent {
	ROOT(SaseSubmissionSystem.APP_PATH, ""), SUBMISSION(
	    SaseSubmissionSystem.APP_PATH, "/submission"), ADMINISTRATION(
	    SaseSubmissionSystem.APP_PATH, "/admin");

	private final String appPath;
	private final String componentPath;

	/**
	 * Creates a new <code>AppComponent</code> object with the given application
	 * path and component path.
	 * 
	 * @param appPath
	 *          the application path
	 * @param componentPath
	 *          the component path
	 */
	private AppComponent(String appPath, String componentPath) {
		this.appPath = appPath;
		this.componentPath = componentPath;
	}

	/**
	 * @return the application path (context path) of this web application
	 *         component
	 */
	public String getAppPath() {
		return appPath;
	}

	/**
	 * @return the component path (/submission, /exam, /admin) of the web
	 *         application component
	 */
	public String getComponentPath() {
		return componentPath;
	}

	/**
	 * @return the absolute path of a component
	 */
	public String getPath() {
		return appPath + componentPath;
	}

	/**
	 * Determines the web application component of a given request URI.
	 * 
	 * @param uri
	 *          the URI to analyze
	 * @return the component, if the given URI points to a part of the SASE SUB
	 *         SYSTEM; null otherwise
	 */
	public static AppComponent getComponent(String uri) {
		uri = uri.replaceFirst("^([\\w\\d+.-]+:)?//[^/]*/", "/");
		if (uri.matches(ROOT.appPath + ".*")) {
			uri = uri.substring(ROOT.appPath.length());
			if (uri.startsWith(SUBMISSION.componentPath + "/")) {
				return SUBMISSION;
			} else if (uri.startsWith(ADMINISTRATION.componentPath + "/")) {
				return ADMINISTRATION;
			} else {
				return ROOT;
			}
		}
		return null;
	}

}
