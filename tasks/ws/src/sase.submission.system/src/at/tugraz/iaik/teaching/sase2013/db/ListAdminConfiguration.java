/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013.db;

import java.io.IOException;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import com.fasterxml.jackson.core.JsonGenerator;

/**
 * Lists all global SASE SUB SYSTEM configuration parameters to a JSON
 * generator. These are only available to a system's administrator.
 * 
 * @author SASE 2013 Team
 */
public class ListAdminConfiguration extends BaseDatabaseOperation {

	/**
	 * Container class for Json field names.
	 * 
	 * @author SASE 2013 Team
	 */
	private static class Json {
		public static final String NUMBER_OF_ENTRIES = "approxEntries";
		public static final String PAGE_MAX = "PageMax";
		public static final String CONFIG_ID = "id";
		public static final String CONFIG_NAME = "name";
		public static final String CONFIG_VALUE = "value";
	}

	private final JsonGenerator g;
	private final int index;
	private final int max;

	/**
	 * Creates a new <code>ListAdminConfiguration</code> with the given JSON
	 * generator, staring index, and number of maximum entries to send to the
	 * client.
	 * 
	 * @param g
	 *          the JSON generator
	 * @param index
	 *          the starting index
	 * @param max
	 *          the maximum number of entries to send to the client
	 */
	public ListAdminConfiguration(JsonGenerator g, int index, int max) {
		super();
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
		int numberOfProps = DbUtils.countGlobalConfigurationProperties(con);
		g.writeNumberField(Json.NUMBER_OF_ENTRIES, numberOfProps);
		g.writeNumberField(Json.PAGE_MAX, max);
		Statement s = con.createStatement();
		try {
			ResultSet rs = s.executeQuery("SELECT * FROM Configuration;");
			if (rs.absolute(index)) {
				int count = 0;
				g.writeFieldName("ConfigData");
				g.writeStartArray();
				do {
					g.writeStartObject();
					g.writeNumberField(Json.CONFIG_ID, rs.getInt(1));
					g.writeStringField(Json.CONFIG_NAME, rs.getString(2));
					g.writeStringField(Json.CONFIG_VALUE, rs.getString(3));
					g.writeEndObject();
					count++;
				} while (rs.next() && count < max);
				g.writeEndArray();
			}
		} finally {
			closeStatement(s);
		}
		throwSqlException();
		g.writeEndObject();
		log.exit();
	}

}
