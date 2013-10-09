package at.tugraz.iaik.teaching.sase2013.db;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import at.tugraz.iaik.teaching.sase2013.SaseSubmissionSystem;

/**
 * Reads the current number of iterations for the password digest function from
 * the SASE SUB SYSTEM database. Call <code>getResult</code> and cast it to an
 * Integer to get the number of iterations.
 * 
 * @author SASE 2013 Team
 */
public class GetPwdIterSpec extends ResultDatabaseOperation {

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
			ps.setString(1, SaseSubmissionSystem.ConfigurationNames.PWD_SEC_SPEC_ITER);
			ps.execute();
			ResultSet rs = ps.getResultSet();
			int iter = 4096;
			if (rs.first()) {
				iter = rs.getInt(1);
			}
			setResult(iter);
		} finally {
			closeStatement(ps);
		}
		throwSqlException();
		log.exit();
	}
}