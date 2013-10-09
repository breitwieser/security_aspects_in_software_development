package at.tugraz.iaik.teaching.sase2013.db;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.text.MessageFormat;

import at.tugraz.iaik.teaching.sase2013.User;

/**
 * Fetches the student role related information for a given user from the SASE
 * SUB SYSTEM database. Currently, this means retrieving the student group
 * number from the database.
 * 
 * Call <code>getResult</code> and cast it to an <code>StudentFacet</code> to
 * get the student related data.
 * 
 * @author SASE 2013 Team
 */
public class GetStudentFacet extends ResultDatabaseOperation {

	private final int uid;

	/**
	 * Creates a new <code>GetStudentFacet</code> object with the given uid.
	 * 
	 * @param uid
	 *          the uid of the user to get the group number for.
	 */
	public GetStudentFacet(int uid) {
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
		    .prepareStatement("SELECT `gid` FROM Students WHERE `uid`=?;");
		try {
			ps.setInt(1, uid);
			ResultSet rs = ps.executeQuery();
			if (!rs.first()) {
				throw new DatabaseOperationException(MessageFormat.format(
				    "User {0} is either no student, or has no group number.", uid));
			}
			setResult(new User.StudentFacet(rs.getInt(1)));
		} finally {
			closeStatement(ps);
		}
		throwSqlException();
		log.exit();
	}
}