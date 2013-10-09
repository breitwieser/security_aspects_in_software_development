/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013.db;

import java.io.IOException;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.LinkedList;

import at.tugraz.iaik.teaching.sase2013.UserRole;

/**
 * Fetches the roles of a specific user from the SASE SUB SYSTEM database.
 * 
 * Call <code>getResult</code> and cast it to a
 * <code>Collection<UserRole></code> to get the user's roles.
 * 
 * @author SASE 2013 Team
 */
public class GetUserRoles extends ResultDatabaseOperation {

	private final int uid;

	/**
	 * Creates a new <code>GetUser</code> object with the given user name.
	 * 
	 * @param userName
	 *          the name of the user to load from the database.
	 */
	public GetUserRoles(int uid) {
		super();
		this.uid = uid;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * at.tugraz.iaik.teaching.sase2013.DatabaseOperation#perform(java.sql.Connection
	 * )
	 */
	@Override
	public void perform(Connection con) throws SQLException, IOException,
	    DatabaseOperationException {
		log.entry();
		PreparedStatement ps = con
		    .prepareStatement("SELECT Roles.`name` FROM Users JOIN (Roles, UserRoles) ON "
		        + "Users.`id`=UserRoles.`uid` AND Roles.`id`=UserRoles.`rid` "
		        + "WHERE Users.`id`=?;");
		try {
			ps.setInt(1, uid);
			ResultSet rs = ps.executeQuery();
			LinkedList<UserRole> roles = new LinkedList<UserRole>();
			while (rs.next()) {
				roles.add(UserRole.getRoleForName(rs.getString(1)));
			}
			setResult(roles);
		} finally {
			closeStatement(ps);
		}
		throwSqlException();
		log.exit();
	}

}
