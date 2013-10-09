/**
 * 
 */
package at.tugraz.iaik.teaching.sase2013;

import java.io.File;

import com.fasterxml.jackson.core.JsonFactory;
import com.fasterxml.jackson.core.JsonGenerator;

/**
 * Container class for SASE_SUB_SYSTEM global configuration values.
 * 
 * @author SASE 2013 Team
 */
public class SaseSubmissionSystem {

	/** The application context path */
	public static final String APP_PATH = "/sase.submission.system";

	/** Name of the request parameter used to control the display index of lists */
	public static final String INDEX_PARAMETER_NAME = "index";

	/** ID of the logger to be used by the SASE SUB SYS web application */
	public static final String LOGGER_ID = "at.tugraz.iaik.teaching.sase2013";

	public static final int DEFAULT_PWD_DIGEST_ITERATIONS = 4096;
	/**
	 * The name of the environment property that contains the system's temporary
	 * directory
	 */
	public final static String TEMP_DIR_PROPERTY = "java.io.tmpdir";
	
	/** A file pointing to the system's temporary directory. */
	public final static File SYSTEM_TEMP_DIR;
	
	/** The prefix for temporary directories created by this application */
	public final static String APP_TEMP_DIR_PREFIX = "SASE_SUB_SYS.";
	
	/**
	 * The JSON factory to use for generating JSON content generators in this web
	 * application. This factory is configured to create JSON generators that do
	 * not to close their output streams. This is a good idea in conjunction with
	 * Servlet output streams.
	 */
	public final static JsonFactory JSON_FACTORY = new JsonFactory();

	static {
		JSON_FACTORY.disable(JsonGenerator.Feature.AUTO_CLOSE_TARGET);
		SYSTEM_TEMP_DIR = new File(System.getProperty(TEMP_DIR_PROPERTY));
	}

	/**
	 * Stores the property names of the SASE SUB SYSTEM configuration properties
	 * @author SASE 2013 Team
	 */
	public static class ConfigurationNames {
		/**
		 * The name of the configuration property that stores the current default
		 * number of iterations for digesting a user specified password.
		 */
		public static final String PWD_SEC_SPEC_ITER = "PwdSecSpecIter";
		/** 
		 * The name of the configuration property that stores the compiler command
		 * used to compile student submissions.
		 */
		public static final String COMPILER_COMMAND = "CompilerCommand";
	}
	
	/**
	 * Stores the attribute names of SASE SUB SYSTEM session attributes.
	 * @author SASE 2013 Team
	 */
	public static class SessionAttribute {

		/** The session attribute to store the user object */
		public static final String USER_OBJECT_ID = "User";
		/** The session attribute to store an error communique */
		public static final String ERROR_COMMUNIQUE = "ErrCom"; 
		
	}
	
}
