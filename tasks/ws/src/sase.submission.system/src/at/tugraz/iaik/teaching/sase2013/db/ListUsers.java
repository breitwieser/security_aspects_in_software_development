/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013.db;

import java.io.IOException;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import com.fasterxml.jackson.core.JsonGenerator;

/**
 * Lists an index selected subset of users stored in the SASE SUB SYSTEM
 * database to a JSON generator. This is only available to the SASE SUB SYSTEM
 * administrators.
 * 
 * @author SASE 2013 Team
 */
public class ListUsers extends BaseDatabaseOperation {

	/**
	 * Container class for Json field names.
	 * @author SASE 2013 Team
	 */
	private static class Json {
		public static final String NUMBER_OF_USERS = "NumberOfUsers";
		public static final String PAGE_MAX = "PageMax";
		public static final String USER_ARRAY_ID = "ListOfUsers";
		public static final String USER_ID = "id";
		public static final String USER_NAME = "name";
		public static final String USER_EMAIL = "email";
		public static final String USER_PWD_ITERATIONS = "iter";
		public static final String USER_PWD_SALT = "salt";
	}

	private final JsonGenerator g;
	private final int index;
	private final int max;

	/**
	 * Creates a new <code>ListUsers</code> database command with the given JSON
	 * generator, starting index and maximum number of entries to send.
	 * 
	 * @param g
	 *          the JSON generator
	 * @param index
	 *          the starting index
	 * @param max
	 *          the maximum number of entries to send to the client
	 */
	public ListUsers(JsonGenerator g, int index, int max) {
		this.g = g;
		this.index = index;
		this.max = max;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * at.tugraz.iaik.teaching.sase2013.DatabaseOperation#perform(java.sql.Connection
	 * )
	 */
	@Override
	public void perform(Connection con) throws SQLException, IOException {
		log.entry();
		g.writeStartObject();
		int numberOfUsers = DbUtils.countUsers(con);
		g.writeNumberField(Json.NUMBER_OF_USERS, numberOfUsers);
		g.writeNumberField(Json.PAGE_MAX, max);
		PreparedStatement ps = con.prepareStatement("SELECT * FROM Users;");
		try {
			ResultSet rs = ps.executeQuery();
			if (rs.absolute(index)) {
				int count = 0;
				g.writeFieldName(Json.USER_ARRAY_ID);
				g.writeStartArray();
				do {
					g.writeStartObject();
					g.writeNumberField(Json.USER_ID, rs.getInt(1));
					g.writeStringField(Json.USER_NAME, rs.getString(2));
					g.writeStringField(Json.USER_EMAIL, rs.getString(3));
					g.writeNumberField(Json.USER_PWD_ITERATIONS, rs.getInt(5));
					g.writeStringField(Json.USER_PWD_SALT, rs.getString(6));
					g.writeEndObject();
					count++;
				} while (rs.next() && count < max);
				g.writeEndArray();
			}
		} finally {
			closeStatement(ps);
		}
		throwSqlException();
		g.writeEndObject();
		log.exit();
	}

}
