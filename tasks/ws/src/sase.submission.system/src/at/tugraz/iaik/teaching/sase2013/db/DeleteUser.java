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
import java.util.LinkedList;

import at.tugraz.iaik.teaching.sase2013.UserRole;

/**
 * Deletes a user with a given id from the SASE SUB SYSTEM database. Deleting a
 * user includes deleting all pertaining entries in the `Students` and `Admins`
 * facet tables, as well as removing all of the user's roles from the `UserRole`
 * table, and finally deleting the user from the `User` table itself.
 * 
 * @author SASE 2013 Team
 */
public class DeleteUser extends BaseDatabaseOperation  {

	private final int id;

	/**
	 * Creates a new <code>DeleteUser</code> object with the given id of the user
	 * to delete.
	 * 
	 * @param id
	 *          the id of the user to delete
	 */
	public DeleteUser(int id) {
		super();
		this.id = id;
	}

	/**
	 * Issues the given update statement with exactly one parameter (the user id).
	 * This method uses a prepared statement to issue the update statement.
	 * 
	 * @param con
	 *          the database connection to use
	 * @param sql
	 *          the SQL statement to execute as prepared statement with one
	 *          integer parameter
	 * @throws SQLException
	 *           if a database access error occurs
	 */
	private void deleteTableEntry(Connection con, String sql) throws SQLException {
		log.entry();
		PreparedStatement del = con.prepareStatement(sql);
		try {
			del.setInt(1, id);
			del.executeUpdate();
		} finally {
			closeStatement(del);
		}
		throwSqlException();
		log.exit();
	}

	/**
	 * Checks if a user with this <code>DeleteUser's</code> user id exists in the
	 * SASE SUB SYSTEM database.
	 * 
	 * @param con
	 *          the database connection to use
	 * @return true if the user exists; false otherwise
	 * @throws SQLException
	 *           if a database access error occurs
	 */
	private boolean existsUser(Connection con) throws SQLException {
		log.entry();
		boolean userExists = false;
		PreparedStatement ps = con.prepareStatement("SELECT COUNT(*) FROM Users "
		    + "WHERE `id`=?;");
		try {
			ps.setInt(1, id);
			ResultSet rs = ps.executeQuery();
			if (rs.first() && rs.getInt(1) > 0) {
				userExists = true;
			}
		} finally {
			closeStatement(ps);
		}
		throwSqlException();
		log.exit();
		return userExists;
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
		int oldLevel = con.getTransactionIsolation();
		con.setTransactionIsolation(Connection.TRANSACTION_READ_COMMITTED);
		try {
			con.setAutoCommit(false);
			try {
				if (!existsUser(con)) {
					return;
				}
				ResultDatabaseOperation rop = new GetUserRoles(id);
				rop.perform(con);
				@SuppressWarnings("unchecked")
				LinkedList<UserRole> roles = (LinkedList<UserRole>) rop.getResult();
				for (UserRole r : roles) {
					switch (r) {
					case ADMIN:
						deleteTableEntry(con, "DELETE FROM Admins WHERE `uid`=?;");
						break;
					case STUDENT:
						deleteTableEntry(con, "DELETE FROM Students WHERE `uid`=?;");
						break;
					default:
						throw new DatabaseOperationException(MessageFormat.format(
						    "Unknown user role {0} for user {1}", r, id));
					}
				}
				deleteTableEntry(con, "DELETE FROM UserRoles WHERE `uid`=?;");
				deleteTableEntry(con, "DELETE FROM Users WHERE `id`=?;");
				con.commit();
				log.exit();
			} finally {
				con.setAutoCommit(true);
			}
		} finally {
			con.setTransactionIsolation(oldLevel);
		}
	}

}
