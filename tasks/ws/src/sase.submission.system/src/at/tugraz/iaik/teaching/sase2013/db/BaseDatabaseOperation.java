/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013.db;

import java.sql.SQLException;
import java.sql.Statement;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import at.tugraz.iaik.teaching.sase2013.SaseSubmissionSystem;

/**
 * <p>
 * Abstract base class for database operations. Provides a way to handle
 * exceptions while closing a statement in a finally block. Each call to
 * closeStatement must be accompanied by a call to throwSqlException. This
 * solution can only handle one exception at the time, if several statements are
 * used in the same operation, take care to always call throwSqlException before
 * issuing the next closeStatement call.
 * </p>
 * 
 * <p>
 * Oh Java 7 where art thou, when we need you (Tomcat 7 only provides Java 6 for
 * JSPs, which makes it impossible to use the new try (closable) {} statement).
 * </p>
 * 
 * @author SASE 2013 Team
 */
public abstract class BaseDatabaseOperation implements DatabaseOperation {

	private SQLException sqlException;

	protected final Logger log = LogManager
	    .getLogger(SaseSubmissionSystem.LOGGER_ID);

	/**
	 * Checks if the given statement is not null and closes the statement if this
	 * is the case. If closing fails due to an exception, this exception is stored
	 * and can be retrieved using the throwSqlException method.
	 * 
	 * @param ps
	 *          the statement to test and close
	 */
	protected void closeStatement(Statement ps) {
		if (ps != null) {
			try {
				ps.close();
			} catch (SQLException e) {
				sqlException = e;
				log.throwing(e);
			}
		}
	}

	/**
	 * Tests if the previous call to closeStatement has lead to an exception being
	 * throw, and if this is the case rethrows this exception.
	 * 
	 * @throws SQLException
	 *           if the previous call to closeStatement stored an exception
	 */
	protected void throwSqlException() throws SQLException {
		if (sqlException != null) {
			throw sqlException;
		}
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
  @Override
  public String toString() {
	  return "BaseDatabaseOperation [sqlException=" + sqlException + ", log="
	      + log + "]";
  }

}
