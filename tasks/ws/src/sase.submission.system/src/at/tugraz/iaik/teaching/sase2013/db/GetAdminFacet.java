package at.tugraz.iaik.teaching.sase2013.db;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import at.tugraz.iaik.teaching.sase2013.User;

/**
 * Fetches the administrator role related information for a given user from the
 * SASE SUB SYSTEM database. Currently, this means retrieving the number of rows
 * to display per table.
 * 
 * Call <code>getResult</code> and cast it to an <code>AdminFacet</code> to get
 * the administrator role related data.
 * 
 * @author SASE 2013 Team
 */
public class GetAdminFacet extends ResultDatabaseOperation {

	private final int uid;

	/**
	 * Creates a new <code>GetAdminFacet</code> object with the given uid.
	 * 
	 * @param uid
	 *          the uid of the user to get the admin role related information for
	 */
	public GetAdminFacet(int uid) {
		super();
		this.uid = uid;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see at.tugraz.iaik.teaching.sase2013.DatabaseOperation#perform(java.sql
	 * .Connection)
	 */
	@Override
	public void perform(Connection con) throws SQLException,
	    DatabaseOperationException {
		log.entry();
		PreparedStatement ps = con
		    .prepareStatement("SELECT `configTableRows`,`userTableRows` FROM Admins WHERE `uid`=?;");
		try {
			ps.setInt(1, uid);
			ResultSet rs = ps.executeQuery();
			if (!rs.first()) {
				/*
				 * Make the administration more resilient by filling in default values
				 * if admin facet isn't found, or inconsistent. Use the GetUserRoles
				 * operation to find out if the user is an admin, or even better the
				 * servlet request isUserInRole function.
				 */
				// throw new DatabaseOperationException(MessageFormat.format(
				// "User {0} is either no administrator, "
				// + "or has no 'configTableRows' property.", uid));
				setResult(new User.AdminFacet(20, 20));
			} else {
				setResult(new User.AdminFacet(rs.getInt(1), rs.getInt(2)));
			}
		} finally {
			closeStatement(ps);
		}
		log.exit();
		throwSqlException();
	}
}