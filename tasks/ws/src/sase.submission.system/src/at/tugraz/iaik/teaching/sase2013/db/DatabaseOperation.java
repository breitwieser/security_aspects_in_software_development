/**
 * Defines database operations, which are database read and/or writes.
 * 
 * <p>Every database access in the SASE SUBMISSION SYSTEM is handled using a
 * <code>DatabaseOperation</code>. The <code>DatabaseOperation</code> provides
 * a <code>Connection</code> and abstracts all the details of obtaining this
 * connection from a database connection pool.</p>
 * 
 * <p>Obtaining database connections from the database connection pool is
 * handled by the <code>DatabaseOperationExecutor</code>. The 
 * <code>DatabaseOperationExecutor</code> is able to execute
 * <code>DatabaseOperations</code>.</p>
 */
package at.tugraz.iaik.teaching.sase2013.db;

import java.io.IOException;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;

/**
 * <p>
 * Allows simple access to the SASE SUB SYSTEM database. The
 * <code>DatabaseOperation</code> abstracts the need to fetch a database
 * connection from the database connection pool and delegates this to the
 * <code>DatabaseOperationExecutor</code>.
 * </p>
 * <p>
 * A database operation can be implemented by overriding the
 * <code>perform</code> method. The perform method is given a {@link Connection}
 * to communicate with the database. The connection is also cleaned up, after
 * the perform method finishes with it. Still, it is advised to close all
 * database resources, such as {@link PreparedStatement}s manually.
 * </p>
 * 
 * @author SASE 2013 Team
 */
public interface DatabaseOperation {

	/**
	 * Performs a database operation. A database operation can be executed via the
	 * {@link DatabaseOperationExecutor}. The database operation is given a
	 * {@link Connection} from a database connection pool to communicate with the
	 * database. Although the connection is cleaned up, before it is returned to
	 * the pool, resources such as {@link PreparedStatement}s should be manually
	 * closed as soon as possible.
	 * 
	 * @param con
	 *          the connection to communicate with the database
	 * @throws DatabaseOperationException
	 *           if a non-SQL/database connection specific error occurs within the
	 *           database operation
	 * @throws SQLException if a database access error occurs
	 * @throws IOException if an I/O problem occurs
	 */
	void perform(Connection con) throws DatabaseOperationException, SQLException,
	    IOException;

}
