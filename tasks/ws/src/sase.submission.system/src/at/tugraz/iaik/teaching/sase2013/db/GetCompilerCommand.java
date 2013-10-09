package at.tugraz.iaik.teaching.sase2013.db;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import at.tugraz.iaik.teaching.sase2013.SaseSubmissionSystem;

/**
 * Reads the compiler command to compile a student submission from the SASE SUB
 * SYSTEM database. Call <code>getResult</code> and cast it to an
 * <code>String</code> to get the compiler command.
 * 
 * @author SASE 2013 Team
 */
public class GetCompilerCommand extends ResultDatabaseOperation {

	/*
	 * (non-Javadoc)
	 * 
	 * @see at.tugraz.iaik.teaching.sase2013.DatabaseOperation#perform(java.sql
	 * .Connection)
	 */
	@Override
	public void perform(Connection con) throws SQLException {
		log.entry();
		PreparedStatement ps = con
		    .prepareStatement("SELECT Configuration.pvalue FROM Configuration "
		        + "WHERE `pname`=?;");
		try {
			ps.setString(1, SaseSubmissionSystem.ConfigurationNames.COMPILER_COMMAND);
			ps.execute();
			ResultSet rs = ps.getResultSet();
			String cc = null;
			if (rs.first()) {
				cc = rs.getString(1);
			}
			setResult(cc);
		} finally {
			closeStatement(ps);
		}
		throwSqlException();
		log.exit();
	}
}