/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013.db;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

/**
 * A set of helper methods to operate on data in the SASE SUB SYSTEM database.
 * 
 * All methods herein accept an SQL connection as parameter. This methods do not
 * manage the SQL connection themselves, meaning the caller has to close the SQL
 * connection after finishing using it. What the utility methods do, is closing
 * all statements they open.
 * 
 * @author SASE 2013 Team
 */
public class DbUtils {

	/**
	 * Counts the number of rows in a given table.
	 * 
	 * @param con
	 *          the JDBC connection to use for database queries. The JDBC
	 *          connection is not closed by this helper method.
	 * @param table
	 *          the name of the table to query
	 * @return the number of rows in the table
	 * @throws SQLException
	 *           if an SQL or database specific error occurs
	 */
	protected static int countRows(Connection con, String table)
	    throws SQLException {
		// TODO Table white list
		PreparedStatement ps = con.prepareStatement("SELECT COUNT(*) FROM " + table
		    + ";");
		try {
			ResultSet rs = ps.executeQuery();
			if (rs.first()) {
				return rs.getInt(1);
			}
		} finally {
			ps.close();
		}
		return 0;
	}

	/**
	 * Determines the number of users registered with the system.
	 * 
	 * @param con
	 *          the JDBC connection to use for database queries. The JDBC
	 *          connection is not closed by this helper method.
	 * @return the number of users registered with the SASE SUB SYSTEM
	 * @throws SQLException
	 *           if an SQL or database specific error occurs
	 */
	protected static int countUsers(Connection con) throws SQLException {
		return countRows(con, "Users");
	}

	/**
	 * Determines the number of global configuration entries in the SASE SUB
	 * SYSTEM database.
	 * 
	 * @param con
	 *          the JDBC connection to use for database queries. The JDBC
	 *          connection is not closed by this helper method.
	 * @return the number of global configuration entries in the SASE SUB SYTEM
	 *         database
	 * @throws SQLException
	 *           if an SQL or database specific error occurs
	 */
	protected static int countGlobalConfigurationProperties(Connection con)
	    throws SQLException {
		return countRows(con, "Configuration");
	}

}
