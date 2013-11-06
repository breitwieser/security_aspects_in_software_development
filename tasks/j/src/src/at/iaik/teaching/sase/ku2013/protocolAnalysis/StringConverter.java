package at.iaik.teaching.sase.ku2013.protocolAnalysis;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.StringReader;
import java.io.StringWriter;

/**
 * @author SASE 2013 Team
 * 
 *         Helper class to convert a ByteArray to a String object and vice
 *         versa.
 */
public class StringConverter {

  /**
   * Takes the byte array {@code input} and converts it to a String array with
   * {@code count} elements.
   * 
   * @param input
   *          The byte array to convert.
   * @param count
   *          The number of elements of the target String array.
   * @return
   * @throws ResourceCheckException
   */
  public static String[] bytesToStringArray(byte[] input, int count)
      throws ResourceCheckException {

    String str = new String(input);

    String[] lines = new String[count];

    StringReader reader = new StringReader(str);
    BufferedReader bufReader = new BufferedReader(reader);
    try {
      int i;
      for (i = 0; i < count; i++) {
        lines[i] = bufReader.readLine();
      }
    } catch (IOException e) {
      throw new ResourceCheckException(
          "StringConverter failed to build a String array.");
    }

    return lines;
  }

  /**
   * Converts a String array to byte array. Always two elements of the byte
   * array are written into one line of the string array.
   * 
   * @param input
   *          the string array to be converted.
   * @param count
   *          the number of lines in the string array.
   * @return
   * @throws ResourceCheckException
   */
  public static byte[] stringToBytes(String[] input, int count)
      throws ResourceCheckException {

    StringWriter writer = new StringWriter();
    BufferedWriter bufWriter = new BufferedWriter(writer);

    try {
      int i;
      for (i = 0; i < (count * 2); i = i + 2) {
        bufWriter.write(input[i]);
        bufWriter.write(input[i + 1]);
        bufWriter.newLine();
      }
      bufWriter.flush();

    } catch (Exception e) {
      throw new ResourceCheckException("Failed to build the request");
    }

    return writer.toString().getBytes();
  }
}
