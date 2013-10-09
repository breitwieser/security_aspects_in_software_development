/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013;

import java.util.HashMap;

/**
 * Pairs an application component with a resource path to uniquely identify an
 * application resource. The idea is to have static registry of all resources
 * (web pages, controllers, ...) and use this for link generation within the
 * application.
 * 
 * @author SASE 2013 Team
 */
public enum AppResource {
	ROOT_INDEX(AppComponent.ROOT, "/index.jsp"),
	ERROR(AppComponent.ROOT, "/error.jsp"),
	LOGIN(AppComponent.ROOT, "/login.jsp"),
	SIGNIN(AppComponent.ROOT, "/signin.jsp"),
	SIGNIN_DO(AppComponent.ROOT, "/signin.do"),
	LOGIN_ERROR_DO(AppComponent.ROOT, "/login-error.do"),
	SUBMISSION_INDEX(AppComponent.SUBMISSION, "/index.jsp"),
	SUBMISSION_DO_UPLOAD(AppComponent.SUBMISSION, "/upload.do"),
	ADMIN_INDEX(AppComponent.ADMINISTRATION, "/index.jsp"),
	ADMIN_CONFIG(AppComponent.ADMINISTRATION, "/config.jsp"),
	ADMIN_UPDATE_CONFIG(AppComponent.ADMINISTRATION, "/config_edit.do"),
	ADMIN_DELETE_USER(AppComponent.ADMINISTRATION, "/user_del.do"),
	ADMIN_USERS(AppComponent.ADMINISTRATION, "/users.jsp"),
	CONFIG_JSON(AppComponent.ROOT, "/config.json"),
	CONDIMENTS_JSON(AppComponent.ROOT, "/condiments.json"),
	USERS_JSON(AppComponent.ROOT, "/users.json"),
	LOGOUT(AppComponent.ROOT, "/logout.do");
	

	/**
	 * A mapping between the servlet path (
	 * <code>HttpServletRequest.getServletPath()</code>) and the
	 * <code>AppResource</code>
	 */
	private static final HashMap<String, AppResource> map;
	private final AppComponent comp;
	private final String path;

	/**
	 * Static initializer block that fills up the servlet path (
	 * <code>HttpServletRequest.getServletPath()</code>) to
	 * <code>AppResource</code> mapping table.
	 */
	static {
		map = new HashMap<String, AppResource>(15);
		for (AppResource r : AppResource.class.getEnumConstants()) {
			map.put(r.getAppPath(), r);
		}
	}

	/**
	 * Constructs a new <code>AppResource</code> object with the given application
	 * component and the specified path.
	 * 
	 * @param comp
	 *          the application component
	 * @param path
	 *          the application path
	 */
	private AppResource(AppComponent comp, String path) {
		this.comp = comp;
		this.path = path;
	}

	/**
	 * @return the application component of this application resource
	 */
	public AppComponent getComponent() {
		return comp;
	}

	/**
	 * @return the path identifying the application resource within the
	 *         application component
	 */
	public String getPath() {
		return path;
	}

	/**
	 * @return the full path that is the application path concatenated with the
	 *         component path concatenated with the resource path
	 */
	public String getFullPath() {
		return comp.getPath() + path;
	}

	/**
	 * @return the path within the application context. This is useful for request
	 *         dispatching
	 */
	public String getAppPath() {
		return comp.getComponentPath() + path;
	}

	/**
	 * Maps a given servlet path to an <code>AppResource</code>, if an
	 * <code>AppResource</code> is registered for this path.
	 * 
	 * @param path
	 *          the servlet path to map to an <code>AppResource</code>
	 * @return the <code>AppResource</code> corresponding to the given servlet
	 *         path, or null if this path does not point to a valid resource of
	 *         this application
	 */
	public static AppResource getResourceFromServletPath(String path) {
		return map.get(path);
	}
}
