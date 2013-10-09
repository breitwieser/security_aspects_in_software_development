/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013;

import java.io.IOException;
import java.io.Serializable;
import java.util.Collection;

import javax.servlet.ServletException;

import at.tugraz.iaik.teaching.sase2013.db.DatabaseOperationExecutor;
import at.tugraz.iaik.teaching.sase2013.db.GetAdminFacet;
import at.tugraz.iaik.teaching.sase2013.db.GetStudentFacet;
import at.tugraz.iaik.teaching.sase2013.db.GetUser;
import at.tugraz.iaik.teaching.sase2013.db.GetUserRoles;
import at.tugraz.iaik.teaching.sase2013.db.ResultDatabaseOperation;

/**
 * A set of classes to represent and access user data.
 * 
 * @author SASE 2013 Team
 */
public class User implements Serializable {

	/**
	 * Auto generated serial verion uid.
	 */
  private static final long serialVersionUID = 4037381164296172367L;

	/**
	 * Encapsulates everything corresponding to the student role of a user.
	 * 
	 * @author SASE 2013 Team
	 */
	public static class StudentFacet implements Serializable {

		/**
		 * Auto generated serial version uid.
		 */
    private static final long serialVersionUID = 3878611936982473751L;
		private final int groupId;

		/**
		 * Creates a new <code>StudentFacet</code> with the given group id.
		 * 
		 * @param groupId
		 *          the group number of the student
		 */
		public StudentFacet(int groupId) {
			this.groupId = groupId;
		}

		/**
		 * @return the group id (number) of the student
		 */
		public int getGroupId() {
			return groupId;
		}

		/* (non-Javadoc)
		 * @see java.lang.Object#hashCode()
		 */
    @Override
    public int hashCode() {
	    final int prime = 31;
	    int result = 1;
	    result = prime * result + groupId;
	    return result;
    }

		/* (non-Javadoc)
		 * @see java.lang.Object#equals(java.lang.Object)
		 */
    @Override
    public boolean equals(Object obj) {
	    if (this == obj)
		    return true;
	    if (obj == null)
		    return false;
	    if (getClass() != obj.getClass())
		    return false;
	    StudentFacet other = (StudentFacet) obj;
	    if (groupId != other.groupId)
		    return false;
	    return true;
    }

		/*
		 * (non-Javadoc)
		 * 
		 * @see java.lang.Object#toString()
		 */
		@Override
		public String toString() {
			return "StudentFacet [group id=" + groupId + "]";
		}

	}

	/**
	 * Encapsulates everything corresponding to the administrator role of a user.
	 * 
	 * @author SASE 2013 Team
	 */
	public static class AdminFacet implements Serializable {

		/**
		 * Auto generated serial version uid.
		 */
    private static final long serialVersionUID = -8037791997332036346L;
		private final int configTableRows;
		private final int userTableRows;

		/**
		 * Creates a new <code>AdminFacet</code> object with the given number of
		 * configuration table rows.
		 * 
		 * @param configTableRows
		 *          the number of rows to show per config table display page.
		 * @param userTableRows
		 * the number of rows to show per user table display page.
		 */
		public AdminFacet(int configTableRows, int userTableRows) {
			super();
			this.configTableRows = configTableRows;
			this.userTableRows = userTableRows;
		}

		/**
		 * @return the number of configuration table rows to display per page
		 */
		public int getConfigTableRows() {
			return configTableRows;
		}

		/**
		 * @return the number of user table rows to display per page
		 */
		public int getUserTableRows() {
			return userTableRows;
		}

		/* (non-Javadoc)
		 * @see java.lang.Object#hashCode()
		 */
    @Override
    public int hashCode() {
	    final int prime = 31;
	    int result = 1;
	    result = prime * result + configTableRows;
	    result = prime * result + userTableRows;
	    return result;
    }

		/* (non-Javadoc)
		 * @see java.lang.Object#equals(java.lang.Object)
		 */
    @Override
    public boolean equals(Object obj) {
	    if (this == obj)
		    return true;
	    if (obj == null)
		    return false;
	    if (getClass() != obj.getClass())
		    return false;
	    AdminFacet other = (AdminFacet) obj;
	    if (configTableRows != other.configTableRows)
		    return false;
	    if (userTableRows != other.userTableRows)
		    return false;
	    return true;
    }

		/* (non-Javadoc)
		 * @see java.lang.Object#toString()
		 */
    @Override
    public String toString() {
	    return "AdminFacet [configTableRows=" + configTableRows
	        + ", userTableRows=" + userTableRows + "]";
    }

	}

	private transient final static DatabaseOperationExecutor exec = DatabaseOperationExecutor
	    .getInstance();

	private final int uid;
	private final String name;
	private final String email;

	/**
	 * Creates a new <code>User</code> object from the given parameters.
	 * 
	 * @param uid
	 *          the unique id of the user (corresponds with database user primary
	 *          key)
	 * @param name
	 *          the name of the user (must not be null or empty)
	 * @param email
	 *          the email address of the user
	 */
	public User(int uid, String name, String email) {
		super();
		this.uid = uid;
		this.name = name;
		this.email = email;
	}

	/**
	 * @return the unique id of the user, which corresponds with the database
	 *         primary key for the users table
	 */
	public int getUid() {
		return uid;
	}

	/**
	 * @return the name of the user
	 */
	public String getName() {
		return name;
	}

	/**
	 * @return the email address of the user
	 */
	public String getEmail() {
		return email;
	}

	/**
	 * @return the student facet of the user if he or she is a student. This
	 *         information is loaded from the SASE SUB SYSTEM database on every
	 *         request.
	 * @throws IOException
	 *           if an I/O problem occurs
	 * @throws ServletException
	 *           if there is a problem while fetching the data from the SASE SUB
	 *           SYSTEM database, or if the user is no student
	 */
	public StudentFacet getStudent() throws ServletException, IOException {
		ResultDatabaseOperation op = new GetStudentFacet(uid);
		exec.executeDbOperation(op);
		return (StudentFacet) op.getResult();
	}

	/**
	 * @return the admin facet of the user if he or she is an administrator. This
	 *         information is loaded from the SASE SUB SYSTEM database anew on
	 *         every request.
	 * @throws IOException
	 *           if an I/O problem occurs
	 * @throws ServletException
	 *           if there is a problem while fetching the data from the SASE SUB
	 *           SYSTEM database, or if the user is no administrator
	 */
	public AdminFacet getAdmin() throws ServletException, IOException {
		ResultDatabaseOperation op = new GetAdminFacet(uid);
		exec.executeDbOperation(op);
		return (AdminFacet) op.getResult();
	}

	/**
	 * @return the roles of the user. This information is loaded from the SASE SUB
	 *         SYSTEM database anew on every request.
	 * @throws IOException
	 *           if an I/O problem occurs
	 * @throws ServletException
	 *           if there is a problem while fetching the data from the SASE SUB
	 *           SYSTEM database, or if the user is no student
	 */
	@SuppressWarnings("unchecked")
	public Collection<UserRole> getRoles() throws ServletException, IOException {
		ResultDatabaseOperation op = new GetUserRoles(uid);
		exec.executeDbOperation(op);
		return (Collection<UserRole>) op.getResult();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#hashCode()
	 */
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + uid;
		return result;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#equals(java.lang.Object)
	 */
	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		User other = (User) obj;
		if (uid != other.uid)
			return false;
		return true;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "User [user id=" + uid + ", name=" + name + ", email=" + email + "]";
	}

	/**
	 * Looks up the given user name in the SASE SUB SYSTEM database and creates a
	 * corresponding <code>User</code> object. To minimize data inconsistencies
	 * the system only loads immutable user data from the database. Currently,
	 * this is the user id, the user name and the user's email address. All other
	 * data will be loaded fresh every time it is requested.
	 * 
	 * @param userName
	 *          the name of the user to look for in the database
	 * @return the <code>User</code> object for the given user name
	 * @throws ServletException
	 *           if a database problem occurs, or the user is not found in the
	 *           database.
	 * @throws IOException
	 *           if an I/O problem occurs
	 */
	public static User loadUser(String userName) throws ServletException,
	    IOException {
		ResultDatabaseOperation rop = new GetUser(userName);
		exec.executeDbOperation(rop);
		return (User) rop.getResult();
	}
}
