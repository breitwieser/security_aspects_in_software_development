/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013.db;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;

/**
 * Updates a configuration value in the SASE SUB SYSTEM database. Only
 * Administrators should be authorized to perform this operation, hence the
 * name.
 * 
 * Configuration entries are just name value pairs, where both name and value
 * are just strings. So no further type checking can be done on the
 * configuration values here.
 * 
 * @author SASE 2013 Team
 */
public class UpdateAdminConfig extends BaseDatabaseOperation {

	private final int id;
	private final String value;

	/**
	 * Creates a new <code>UpdateAdminConfig</code> object with the given id and
	 * value.
	 * 
	 * @param id
	 *          the id of the record to update
	 * @param value
	 *          the new value for the record
	 */
	public UpdateAdminConfig(int id, String value) {
		super();
		this.id = id;
		this.value = value;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * at.tugraz.iaik.teaching.sase2013.DatabaseOperation#perform(java.sql.Connection
	 * )
	 */
	@Override
	public void perform(Connection con) throws SQLException {
		log.entry();
		PreparedStatement ps = con
		    .prepareStatement("UPDATE Configuration SET pvalue=? WHERE id=?");
		try {
			ps.setString(1, value);
			ps.setInt(2, id);
			ps.executeUpdate();
		} finally {
			closeStatement(ps);
		}
		throwSqlException();
		log.exit();
	}

}
