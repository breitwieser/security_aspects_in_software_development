package at.tugraz.iaik.teaching.sase2013.db;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

/**
 * Tests if a specific user with the given name already exists in the SASE SUB
 * SYSTEM database. If so <code>getResult</code> returns true; false otherwise
 * (Cast to a Boolean first).
 * 
 * @author SASE 2013 Team
 */
public class ExistsUserId extends ResultDatabaseOperation {
	
	private final String name;

	/**
	 * Create a new <code>ExistsUser</code> object to look for a user with the
	 * given name.
	 * 
	 * @param name
	 *          the user name
	 */
	public ExistsUserId(final String name) {
		this.name = name;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see at.tugraz.iaik.teaching.sase2013.DatabaseOperation#perform(java.sql
	 * .Connection)
	 */
	@Override
	public void perform(Connection con) throws SQLException {
		log.entry();
		PreparedStatement ps = con.prepareStatement("SELECT COUNT(*) FROM Users "
		    + "WHERE `name`=?;");
		try {
			ps.setString(1, name);
			ps.execute();
			ResultSet rs = ps.getResultSet();
			if (rs.first() && rs.getInt(1) > 0) {
				// User already exists
				setResult(Boolean.TRUE);
			} else {
				setResult(Boolean.FALSE);
			}
		} finally {
			closeStatement(ps);
		}
		throwSqlException();
		log.exit();
	}
}