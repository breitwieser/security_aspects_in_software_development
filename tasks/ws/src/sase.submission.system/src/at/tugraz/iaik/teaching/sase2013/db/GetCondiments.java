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
 * Retrieves the salt and number of iterations needed to compute a specific
 * user's password from the SASE SUB SYS database. This is crucial for login.
 * 
 * @author SASE 2013 Team
 */
public class GetCondiments extends BaseDatabaseOperation {

	/**
	 * Container class for Json field names.
	 * 
	 * @author SASE 2013 Team
	 */
	private static class Json {
		public static final String SALT = "salt";
		public static final String ITER = "iter";
	}

	private final JsonGenerator g;
	private final String name;

	/**
	 * Creates a new <code>GetCondiments</code> object from the given JSON
	 * generator and user name.
	 * 
	 * @param g
	 *          the JSON generator
	 * @param name
	 *          the name of the user to retrieve the condiments for
	 */
	public GetCondiments(JsonGenerator g, String name) {
		super();
		this.g = g;
		this.name = name;
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
		PreparedStatement ps = con
		    .prepareStatement("SELECT `salt`, `iter` FROM Users WHERE name=?;");
		try {
			ps.setString(1, name);
			ResultSet rs = ps.executeQuery();
			if (rs.first()) {
				g.writeStringField(Json.SALT, rs.getString(1));
				g.writeNumberField(Json.ITER, rs.getInt(2));
			}
		} finally {
			closeStatement(ps);
		}
		throwSqlException();
		g.writeEndObject();
		log.exit();
	}

}
