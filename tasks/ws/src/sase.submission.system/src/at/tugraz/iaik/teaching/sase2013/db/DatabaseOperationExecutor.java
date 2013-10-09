/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013.db;

import java.io.IOException;
import java.sql.Connection;
import java.sql.SQLException;

import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import javax.servlet.ServletException;
import javax.sql.DataSource;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import at.tugraz.iaik.teaching.sase2013.SaseSubmissionSystem;

/**
 * Executes database operations. Handles obtaining a data source, which can
 * execute database statements.
 * 
 * This instances fetches the data source on a per call basis, which is probably
 * wasteful, but should also be inherently thread safe.
 * 
 * @author SASE 2013 Team
 */
public class DatabaseOperationExecutor {

	private final Logger log = LogManager
	    .getLogger(SaseSubmissionSystem.LOGGER_ID);
	private static final DatabaseOperationExecutor instance = new DatabaseOperationExecutor();

	/**
	 * Executes a <code>DatabaseOperation</code>. A <code>DatabaseOperation</code>
	 * relies on an open connection to the database backing the <i>SASE SUB
	 * SYSTEM</i> application. This method encapsulates all the ugly get database
	 * connection via a data source code, which in turn is specified by the
	 * Servlet container (see <code>META-INF/web.xml</code>) and the associated
	 * error handling and safe resource destruction. It just gives the database
	 * operation a nice <code>Connection</code> object to work with.
	 * 
	 * @param op
	 *          the database operation to execute
	 * @throws ServletException
	 *           if anything goes wrong during obtaining a database connection, or
	 *           if the <code>DatabaseOperation</code> does not handle its
	 *           <code>SQLExceptions</code>
	 * @throws IOException
	 *           if an I/O problem occurs in the <code>DatabaseOperation</code>
	 */
	public void executeDbOperation(DatabaseOperation op) throws ServletException,
	    IOException {
		Context envContext = null;
		try {
			envContext = new InitialContext();
			DataSource ds = (DataSource) envContext
			    .lookup("java:/comp/env/jdbc/sasesubsystem");
			try (Connection con = ds.getConnection()) {
				op.perform(con);
			} catch (SQLException e) {
				/*
				 * the following three exceptions are very bad, not much we can do about
				 * then, except logging
				 */
				log.catching(e);
				throw new ServletException(e);
			} catch (DatabaseOperationException e) {
				log.catching(e);
				throw new ServletException(e);
			}
		} catch (NamingException e) {
			log.catching(e);
			throw new ServletException(e);
		} finally {
			if (envContext != null) {
				try {
					envContext.close();
				} catch (NamingException e) {
					log.error("JNDI Environmental context could not be closed!", e);
				}
			}
		}
	}

	/**
	 * Returns the singleton instance of the
	 * <code>DatabaseOperationExecutor</code>.
	 * 
	 * @return the singleton instance of the
	 *         <code>DatabaseOperationExecutor</code>.
	 */
	public static DatabaseOperationExecutor getInstance() {
		return instance;
	}

}
