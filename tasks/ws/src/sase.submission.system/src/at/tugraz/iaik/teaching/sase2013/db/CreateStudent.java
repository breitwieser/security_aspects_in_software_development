/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013.db;

import java.io.IOException;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.text.MessageFormat;

import at.tugraz.iaik.teaching.sase2013.UserRole;

/**
 * Creates a new Student account in the SASE SUB SYSTEM database. This operation
 * tries to perform all required <b>insert</b> operations in one single
 * transaction to prevent database inconsistencies.
 * 
 * @author SASE 2013 Team
 */
public class CreateStudent extends BaseDatabaseOperation {

	private final String name;
	private final String email;
	private final String pass;
	private final int iter;
	private final String salt;
	private final int group;

	/**
	 * Creates a new <code>CreateStudent</code> object from the given parameters.
	 * <p>
	 * Note: No need to do parameter sanity checking here because it is done in
	 * the calling code.
	 * </p>
	 * 
	 * @param name
	 *          the name of the user
	 * @param email
	 *          the mail address of the user
	 * @param pass
	 *          the password of the user
	 * @param iter
	 *          the number of iterations for the password digest function
	 * @param salt
	 *          the salt for the password digest function
	 * @param group
	 */
	public CreateStudent(String name, String email, String pass, int iter,
	    String salt, int group) {
		super();
		this.name = name;
		this.email = email;
		this.pass = pass;
		this.iter = iter;
		this.salt = salt;
		this.group = group;
	}

	/**
	 * Inserts the User table part of a student into the database.
	 * 
	 * @param con
	 *          the connection to use for SQL statements
	 * @return the unique id of the user (primary key)
	 * @throws SQLException
	 *           if a database access problem occurs
	 * @throws DatabaseOperationException
	 *           if the user cannot be created
	 */
	private int insertUser(Connection con) throws SQLException,
	    DatabaseOperationException {
		log.entry();
		int userId = -1;
		PreparedStatement ps = con.prepareStatement(
		    "INSERT INTO Users(name,email,pass,iter,salt) " + "VALUES(?,?,?,?,?);",
		    Statement.RETURN_GENERATED_KEYS);
		try {
			ps.setString(1, name);
			ps.setString(2, email);
			ps.setString(3, pass);
			ps.setInt(4, iter);
			ps.setString(5, salt);
			ps.executeUpdate();
			ResultSet rs = ps.getGeneratedKeys();
			if (rs == null || !rs.first()) {
				throw new DatabaseOperationException(MessageFormat.format(
				    "Failed to create student entry {0}, {1}", name, email));
			}
			userId = rs.getInt(1);
		} finally {
			closeStatement(ps);
		}
		throwSqlException();
		log.exit();
		return userId;
	}

	/**
	 * Insert the student facet (Student table) part of the student entity.
	 * 
	 * @param con
	 *          the connection to use for SQL statements
	 * @param userId
	 *          the unique id of the user (primary key of the User table)
	 * @throws SQLException
	 *           if a database access problem occurs
	 */
	private void insertStudentFacet(Connection con, int userId)
	    throws SQLException {
		log.entry();
		PreparedStatement ps = con
		    .prepareStatement("INSERT INTO Students(`uid`,`gid`) VALUES(?,?);");
		try {
			ps.setInt(1, userId);
			ps.setInt(2, group);
			ps.executeUpdate();
		} finally {
			closeStatement(ps);
		}
		throwSqlException();
		log.exit();
	}

	/**
	 * Inserts the student role for the given user in the UserRoles table.
	 * 
	 * @param con
	 *          the connection to use for SQL statements
	 * @param userId
	 *          the unique id of the user (primary key of the User table)
	 * @param studentRoleId
	 *          the unique id of the student role
	 * @throws SQLException
	 *           if a database access problem occurs
	 */
	private void insertStudentRole(Connection con, int userId, int studentRoleId)
	    throws SQLException {
		log.entry();
		PreparedStatement ps = con
		    .prepareStatement("INSERT INTO UserRoles(`uid`,`rid`) VALUES(?,?);");
		try {
			ps.setInt(1, userId);
			ps.setInt(2, studentRoleId);
			ps.executeUpdate();
		} finally {
			closeStatement(ps);
		}
		throwSqlException();
		log.exit();
	}

	/**
	 * Insert the user and adds the student facet using a transaction.
	 * <p>
	 * Note: No need to do parameter sanity checking here because it is done in
	 * the calling code.
	 * </p>
	 * 
	 * @param con
	 *          the connection to use for SQL statements
	 * @param studentRoleId
	 *          the id of Student role in the database
	 * @throws SQLException
	 *           if an SQL problem occurs
	 * @throws DatabaseOperationException
	 *           if the creation of a user entry fails
	 */
	private void insertStudent(Connection con, int studentRoleId)
	    throws SQLException, DatabaseOperationException {
		log.entry();
		try {
			con.setAutoCommit(false);
			int userId = insertUser(con);
			insertStudentFacet(con, userId);
			insertStudentRole(con, userId, studentRoleId);
			con.commit();
		} finally {
			con.setAutoCommit(true);
		}
		log.exit();
	}

	/**
	 * Returns the student role id that is the unique key of the student role.
	 * 
	 * @param con
	 *          the connection to use for SQL statements
	 * @return the unique key of the student role
	 * @throws SQLException
	 *           if a database access problem occurs
	 * @throws DatabaseOperationException
	 *           if the student role does not exist in the database
	 */
	private int getStudentRole(Connection con) throws SQLException,
	    DatabaseOperationException {
		int studentRoleId = -1;
		PreparedStatement ps = con
		    .prepareStatement("SELECT `id` FROM Roles WHERE `name`=?;");
		try {
			ps.setString(1, UserRole.STUDENT.getRoleName());
			/*
			 * TODO TOCTOU issue: User role id might change between the above select
			 * and the transaction protected insert below. Please don't change the
			 * student role id while the system is deployed!
			 */
			ResultSet rs = ps.executeQuery();
			if (!rs.first()) {
				throw new DatabaseOperationException(MessageFormat.format(
				    "Failed to read {0} role id from database", UserRole.STUDENT));
			}
			studentRoleId = rs.getInt(1);
		} finally {
			closeStatement(ps);
		}
		throwSqlException();
		log.exit();
		return studentRoleId;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * at.tugraz.iaik.teaching.sase2013.DatabaseOperation#perform(java.sql.Connection
	 * )
	 */
	@Override
	public synchronized void perform(Connection con) throws SQLException,
	    IOException, DatabaseOperationException {
		log.entry();
		try {
			int studentRoleId = getStudentRole(con);
			insertStudent(con, studentRoleId);
		} finally {
			con.setAutoCommit(true);
		}
		log.exit();

	}

}
