package at.tugraz.iaik.teaching.sase2013.db;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.text.MessageFormat;

/**
 * Tests if a specific user with the given name already exists in the SASE SUB
 * SYSTEM database. If so <code>getResult</code> returns the name of the user if
 * she exists; null otherwise (Cast to a String first).
 * 
 * @author SASE 2013 Team
 */
public class ExistsUserName extends ResultDatabaseOperation {

	private final String name;

	/**
	 * Create a new <code>ExistsUser</code> object to look for a user with the
	 * given name.
	 * 
	 * @param name
	 *          the name of the user to look for
	 */
	public ExistsUserName(final String name) {
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
		//PreparedStatement ps = con.prepareStatement(MessageFormat.format(
		//    "SELECT `name` FROM Users WHERE `name`=''{0}'';", name));
		//Fix 2e
		PreparedStatement ps = con.prepareStatement("SELECT `name` FROM Users WHERE `name`=?;");
		try {
			//Fix 2e
			ps.setString(1, name);
			ps.execute();
			ResultSet rs = ps.getResultSet();
			String user = new String();
			while (rs.next()) {
				user += rs.getString(1);
			}
			setResult(user);
		} finally {
			closeStatement(ps);
		}
		throwSqlException();
		log.exit();
	}
}