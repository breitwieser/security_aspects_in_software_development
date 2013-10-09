/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013.db;

/**
 * Adds the ability to remember a result for a database operation. This is
 * useful for database operations that read data from the database. 
 * 
 * @author SASE 2013 Team
 */
public abstract class ResultDatabaseOperation extends BaseDatabaseOperation {

	private Object result = null;

	/**
	 * Retrieves the result of the database operation.
	 * @return the result of the database operation
	 */
	public Object getResult() {
		return result;
	}

	/**
	 * Sets the result of the database operations.
	 * @param result
	 *          the result of the database operation
	 */
	protected void setResult(Object result) {
		this.result = result;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "ResultDatabaseOperation [result=" + result + "]";
	}

}
