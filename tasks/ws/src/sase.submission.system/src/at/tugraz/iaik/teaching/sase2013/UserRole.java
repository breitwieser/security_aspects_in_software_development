/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013;

/**
 * Enumeration of user roles known to the SASE SUB SYSTEM.
 * 
 * The SASE SUB SYSTEM knows to user roles:
 * <ul>
 * <li>Student: A student who wants to submit a program for the SASE course.</li>
 * <li>Administrator: Someone who is allowed to administer (configure and
 * manage) the SASE SUB SYSTEM.</li>
 * </ul>
 * 
 * @author SASE 2013 Team
 */
public enum UserRole {
	STUDENT("Student"), ADMIN("Administrator");

	private final String roleName;

	/**
	 * Creates a new <code>UserRole</code> object with the given role name.
	 * 
	 * @param roleName
	 *          the name of the role, as it is used to identify the role in the
	 *          SASE SUB SYSTEM database
	 */
	private UserRole(String roleName) {
		this.roleName = roleName;
	}

	/**
	 * @return the roleName the name of the role, as it is used to identify the
	 *         role in the SASE SUB SYSTEM database
	 */
	public String getRoleName() {
		return roleName;
	}

	/**
	 * Translates a given role name to its corresponding <code>UserRole</code>
	 * object.
	 * 
	 * @param name
	 *          the role name
	 * @return the user role corresponding to the given role name, or null if the
	 *         role name is not known
	 */
	public static UserRole getRoleForName(String name) {
		if (name.equals(STUDENT.getRoleName())) {
			return STUDENT;
		} else if (name.equals(ADMIN.getRoleName())) {
			return ADMIN;
		} else {
			return null;
		}
	}
}
