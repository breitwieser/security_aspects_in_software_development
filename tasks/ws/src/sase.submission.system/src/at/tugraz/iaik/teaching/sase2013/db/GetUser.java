/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013.db;

import java.io.IOException;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.text.MessageFormat;

import at.tugraz.iaik.teaching.sase2013.User;

/**
 * Tries to load the user with the given name from the SASE SUB SYSTEM database.
 * 
 * Call <code>getResult</code> and cast it to a <code>User</code> to get access
 * to the user data.
 * 
 * @author SASE 2013 Team
 */
public class GetUser extends ResultDatabaseOperation {

	private final String userName;

	/**
	 * Creates a new <code>GetUser</code> object with the given user name.
	 * 
	 * @param userName
	 *          the name of the user to load from the database.
	 */
	public GetUser(String userName) {
		super();
		this.userName = userName;
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
		    .prepareStatement("SELECT * FROM Users WHERE `name`=?;");
		try {
			ps.setString(1, userName);
			ResultSet rs = ps.executeQuery();
			if (!rs.first()) {
				throw new DatabaseOperationException(MessageFormat.format(
				    "No user {0} found in database.", userName));
			}
			int uid = rs.getInt(1);
			String name = rs.getString(2);
			String email = rs.getString(3);
			setResult(new User(uid, name, email));
		} finally {
			closeStatement(ps);
		}
		throwSqlException();
		log.exit();
	}

}
