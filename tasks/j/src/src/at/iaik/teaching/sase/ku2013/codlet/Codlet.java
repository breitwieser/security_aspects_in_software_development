package at.iaik.teaching.sase.ku2013.codlet;

import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.Properties;

import static iaik.utils.Util.readFile;

/**
 * Processes the binary received from the server. This is obviously a dummy
 * class for pretending to process the received binary.
 * 
 * @author SASE 2013 Team
 * 
 */
public class Codlet {

  public Codlet() {
  }

  /**
   * Loads the original binary from file and compares it to the received one.
   * 
   * @param bin
   *          the binary
   * @throws IOException
   */
  public void processBinary(Properties cfg, File base_dir, byte[] recv)
      throws IOException {

    byte[] bin = recv.clone();

    if (bin != null) {
      String binary_file = cfg.getProperty("client.binary");
      byte[] binary = null;
      if (binary_file != null) {
        binary = readFile(binary_file);

        if (Arrays.equals(bin, binary))
          System.out.println("Submission successful...");

        else
          System.out.println("Binary is different from client's binary");
      }
    } else
      System.out.println("Submission error...");
  }
}
