package at.iaik.teaching.sase.ku2013.test;

import static at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox.KEY_ALG_AES;
import static at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox.KEY_ALG_HMAC_SHA1;
import static at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox.aesCcmDecrypt;
import static at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox.aesCcmEncrypt;
import static at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox.computeHmac;
import static at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox.generateRsaKeyPair;
import static at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox.generateSecretKey;
import static at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox.getRandomBytes;
import static at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox.importSecretKey;
import static at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox.unWrapKey;
import static at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox.verifyHmac;
import static at.iaik.teaching.sase.ku2013.crypto.CryptoToolBox.wrapKey;
import static iaik.utils.Util.decodeByteArray;
import iaik.security.provider.IAIK;

import java.nio.charset.Charset;
import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.spec.PKCS8EncodedKeySpec;
import java.security.spec.X509EncodedKeySpec;
import java.util.Arrays;

import javax.crypto.SecretKey;

import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

/**
 * Simple test cases for the crypto toolbox.
 */
public class CryptoTests {

  /**
   * Plaintext for AES fixed key encryption/decryption test.
   */
  private static final String TXT_AES_PLAINTEXT_A = "This time with a fixed message.";

  /**
   * Base64 encoded ciphertext for AES fixed key encryption/decryption test.
   */
  private static final String B64_AES_CIPHERTEXT_A = "/gzyGvpyxQSl6h8TAZHdKcdNr8k8BcKLZf4ATihdqGw19fgiHDlBSaLv2Q==";

  /**
   * Base64 encoded AES 128 test key.
   */
  private static final String B64_AES_KEY_A = "MDEyMzQ1Njc4OUFCQ0RFRg==";

  /**
   * Base64 encoded HMAC-SHA1 test key.
   */
  private static final String B64_KEY_HMAC_A = "MDEyMzQ1Njc4OUFCQ0RFRjAxMjM0NTY3ODlBQkNERUY=";

  /**
   * Base64 encoded value of the HMAC-SHA1 test with fixed key.
   */
  private static final String B64_HMAC_EXPECTED_A = "JeuYhbza85mE+ZwmMciC9jgSNQo=";

  /**
   * Hello world message.
   */
  private static final String TXT_HELLO_WORLD = "Hello world, this is a simple test message!";

  /**
   * Another test message.
   */
  private static final String TXT_ANOTHER_MESSAGE = "Yet another opportunity for testing it!";

  /**
   * It is "time to hack" test message.
   */
  private static final String TXT_TIME_TO_HACK = "It is time to HACK!";

  /**
   * It is "time to test" test message.
   */
  private static final String TXT_TIME_TO_TEST = "It is time to test!";

  /**
   * UTF-8 character set (used for encoding strings as byte arrays)
   */
  private static final Charset CHARSET_UTF8 = Charset.forName("UTF-8");

  /**
   * Base64 encoded RSA public key.
   * 
   * The encoded byte array is intended to be used with the {@link KeyFactory}
   * and the {@link X509EncodedKeySpec} classes.
   */
  private static final String B64_RSA_PUBLIC_KEY = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCCHJ0bFypP08b1fEnY+t86x3MD"
      + "pN7JDzs+0wdW7WtD3IPxXCZ+WaxCx2IgwV//KdU/m2UAw1Yj0/vrzruVqFvA0dIR"
      + "635P0tntQyRFQBVOJO+IbaTkp9gw8P1xVnmZddkgb0cZmlIaS3wV7Ti0M3HmEf0K"
      + "vuI3M1aDDzIayyoYcQIDAQAB";

  /**
   * Base64 encoded RSA private key.
   * 
   * The encoded byte array is intended to be used with the {@link KeyFactory}
   * and the {@link PKCS8EncodedKeySpec} classes.
   */
  private static final String B64_RSA_PRIVATE_KEY = "MIICdQIBADANBgkqhkiG9w0BAQEFAASCAl8wggJbAgEAAoGBAIIcnRsXKk/TxvV8"
      + "Sdj63zrHcwOk3skPOz7TB1bta0Pcg/FcJn5ZrELHYiDBX/8p1T+bZQDDViPT++vO"
      + "u5WoW8DR0hHrfk/S2e1DJEVAFU4k74htpOSn2DDw/XFWeZl12SBvRxmaUhpLfBXt"
      + "OLQzceYR/Qq+4jczVoMPMhrLKhhxAgMBAAECgYB+ku13LDr6O3Bi4g1JKwy8tsi5"
      + "s8sTV2DwRgTu2Lqv4rbstxOvePbuY96fNN95cC3G2hoQ+vYyloeNp6iu94CBmUkO"
      + "6vgCmVohjaVmWIrY35XPL1ycQEL8tQZVR4UcuEJtt1rWfHO/ahKKy05/iuJY8KAn"
      + "psNi29/vudu9gW+tfQJBAMBIP3sn8x4wY+jqzT/RjSDTtTnqNlc22kazNpNOIAgY"
      + "rPTONZV3C145fqVb5RHtZC9pbLGoJEqvKcyO8Z2IcmMCQQCtOk0A/9pJ/KnC6t4u"
      + "MAyC5jqPoT23aB6fY1gvAESZ2yMo6Od1f3ry9D9gdqqsgIlwJnRTAoTJpt9YrxKv"
      + "/FgbAkA8IBthmltmbEFBmvJ1a0RTKIA5zOgTG37jrrR5GWvgvD6RCk4tH515aEcW"
      + "1AZsNawnOzMW1noKtH1Ajs40sW77AkBDyPWK95NGsm0HjfqFHl429rphq9nHpCGf"
      + "gzNlqPgvMHYQsxqnesv0uCj4qVbeC6o8TX/lX63wbrElNdTdMqSTAkBytSgcD+9L"
      + "m6u2fDyR+gtUpEHjiJV0CLLerAeCogCeYYS+53IowfMCeXoHjAbktSCU3PB8xUOb"
      + "jG5SFA5wMAGC";

  /**
   * Base64 encoded byte array containing the wrapped AES test key A.
   * 
   * This encoded byte array has been produced by wrapping the AES test key "A"
   * with the fixed public key shown above.
   */
  private static final String B64_WRAPPED_AES_KEY_A = "XFoJ2jsuJcm+GSQPrp8g+GPneKuk2/1+FZO2pNzOK5TKRfv+MV28bLn0ITP4tRVJ"
      + "Tx27NF4Ek+SRCKW9s5/kTmZfcLKkYVXv25OZGP5QimE2w0K2/z83Of8nJH6sEFbw"
      + "9c/CCH3E7ZDnfbz4IzTOeG7OEJ4zMVV57RzNqpTMvuY=";

  @Before
  public void setUp() {
    // Force registration of the IAIK provider
    IAIK.addAsProvider();
  }

  @Test
  public void aesRandomKey() throws Exception {
    // AES-128 CCM encryption with random key and iv
    SecretKey key = generateSecretKey(KEY_ALG_AES, 128);
    byte[] iv = getRandomBytes(13);
    byte[] plaintext = TXT_HELLO_WORLD.getBytes(CHARSET_UTF8);

    try
    {
    	
//    System.out.println("aesRandomKey");
//    System.out.println(Arrays.toString(plaintext));
    
    // Encrypt once
    byte[] ciphertext = aesCcmEncrypt(plaintext, 0, plaintext.length, key, iv);

//    System.out.println(Arrays.toString(ciphertext));
    
    // Decrypt once
    byte[] decrypted = aesCcmDecrypt(ciphertext, 0, ciphertext.length, key, iv);

//    System.out.println(Arrays.toString(decrypted));
    
    // And check
    Assert.assertArrayEquals(plaintext, decrypted);
    }catch(Exception e){e.printStackTrace();}
  }

  @Test
  public void aesFixedKey() throws Exception {
    // Our fixed 128-bit AES key
    SecretKey key = importSecretKey(KEY_ALG_AES, decodeByteArray(B64_AES_KEY_A));

    // AES-128 CCM encryption with fixed key and zero IV.
    byte[] iv = new byte[13];
    byte[] plaintext = TXT_AES_PLAINTEXT_A.getBytes(CHARSET_UTF8);
    byte[] reference = decodeByteArray(B64_AES_CIPHERTEXT_A);
    // Encrypt once
    byte[] ciphertext = aesCcmEncrypt(plaintext, 0, plaintext.length, key, iv);
    Assert.assertArrayEquals(reference, ciphertext);

    // Decrypt again
    byte[] decrypted = aesCcmDecrypt(ciphertext, 0, ciphertext.length, key, iv);
    Assert.assertArrayEquals(plaintext, decrypted);

    // Encrypt the reference ciphertext a second time, then decrypt twice to
    // obtain the plaintext
    byte[] double_ciphertext = aesCcmEncrypt(reference, 0, reference.length,
        key, iv);
    byte[] single_ciphertext = aesCcmDecrypt(double_ciphertext, 0,
        double_ciphertext.length, key, iv);
    Assert.assertArrayEquals(reference, single_ciphertext);

    byte[] double_plaintext = aesCcmDecrypt(single_ciphertext, 0,
        single_ciphertext.length, key, iv);
    Assert.assertArrayEquals(plaintext, double_plaintext);
  }

  @Test
  public void hmacRandomKey() throws Exception {
    // Our random 160-bit HMAC-SHA1 key
    SecretKey key = generateSecretKey(KEY_ALG_HMAC_SHA1, 160);

    byte[] first_message = TXT_TIME_TO_TEST.getBytes(CHARSET_UTF8);
    byte[] second_message = TXT_TIME_TO_HACK.getBytes(CHARSET_UTF8);

    byte[] first_mac = computeHmac(first_message, 0, first_message.length, key);
    byte[] second_mac = computeHmac(second_message, 0, second_message.length,
        key);

    // Simple test (good)
    Assert.assertTrue(verifyHmac(first_message, 0, first_message.length, key,
        first_mac));
    Assert.assertTrue(verifyHmac(second_message, 0, first_message.length, key,
        second_mac));

    // Simple test (mismatch)
    Assert.assertFalse(verifyHmac(first_message, 0, first_message.length, key,
        second_mac));
    Assert.assertFalse(verifyHmac(second_message, 0, first_message.length, key,
        first_mac));
  }

  @Test
  public void hmacFixedKey() throws Exception {
    // Fixed test key
    SecretKey key = importSecretKey(KEY_ALG_HMAC_SHA1,
        decodeByteArray(B64_KEY_HMAC_A));

    // Fixed test data
    byte[] message = TXT_ANOTHER_MESSAGE.getBytes(CHARSET_UTF8);
    byte[] reference = decodeByteArray(B64_HMAC_EXPECTED_A);
    byte[] mac = computeHmac(message, 0, message.length, key);

    // Simple test (good)
    Assert.assertArrayEquals(mac, reference);
    Assert.assertTrue(verifyHmac(message, 0, message.length, key, mac));
  }

  @Test
  public void randomKeyWrapUnwrap() throws Exception {
    // Generate a random RSA key-pair
    KeyPair keypair = generateRsaKeyPair(1024);

    // Generate a random AES 128 encryption key
    SecretKey secret_key = generateSecretKey(KEY_ALG_AES, 128);

    // Encrypt (wrap) the secret key with the RSA public key
    PublicKey public_key = keypair.getPublic();
    byte[] wrapped_blob = wrapKey(secret_key, public_key);

    // Decrypt (unwrap) the wrapped key blob with the RSA private key
    PrivateKey private_key = keypair.getPrivate();
    SecretKey unwrapped_secret_key = unWrapKey(wrapped_blob, KEY_ALG_AES,
        private_key);

    // Compare the encoded (byte array) form of the keys - they must be
    // equal.
    Assert.assertArrayEquals(secret_key.getEncoded(),
        unwrapped_secret_key.getEncoded());
  }

  @Test
  public void fixedKeyWrapUnwrap() throws Exception {
    // Load the fixed keypair for testing.
    KeyFactory factory = KeyFactory.getInstance("RSA", "IAIK");

    // Construct the private key from its byte array
    PKCS8EncodedKeySpec private_spec = new PKCS8EncodedKeySpec(
        decodeByteArray(B64_RSA_PRIVATE_KEY));
    PrivateKey private_key = factory.generatePrivate(private_spec);

    // Construct the public key from its byte array
    X509EncodedKeySpec public_spec = new X509EncodedKeySpec(
        decodeByteArray(B64_RSA_PUBLIC_KEY));
    PublicKey public_key = factory.generatePublic(public_spec);

    // Load the fixed secret key for testing
    SecretKey secret_key = importSecretKey(KEY_ALG_AES,
        decodeByteArray(B64_AES_KEY_A));

    // Encrypt (wrap) the secret key with the RSA public key
    byte[] wrapped_blob = wrapKey(secret_key, public_key);

    // Decrypt (unwrap) the wrapped key blob with the RSA private key
    SecretKey unwrapped_secret_key = unWrapKey(wrapped_blob, KEY_ALG_AES,
        private_key);

    // Compare the encoded (byte array) form of the keys - they must be
    // equal.
    Assert.assertArrayEquals(secret_key.getEncoded(),
        unwrapped_secret_key.getEncoded());
  }

  @Test
  public void fixedKeyUnwrap() throws Exception {
    // Load the fixed keypair for testing.
    KeyFactory factory = KeyFactory.getInstance("RSA", "IAIK");

    // Construct the private key from its byte array
    PKCS8EncodedKeySpec private_spec = new PKCS8EncodedKeySpec(
        decodeByteArray(B64_RSA_PRIVATE_KEY));
    PrivateKey private_key = factory.generatePrivate(private_spec);

    // Decode the test vectors
    byte[] wrapped_blob = decodeByteArray(B64_WRAPPED_AES_KEY_A);
    byte[] reference_key = decodeByteArray(B64_AES_KEY_A);

    // Decrypt (unwrap) the wrapped key blob with the RSA private key
    byte[] unwrapped_secret_key = unWrapKey(wrapped_blob, KEY_ALG_AES,
        private_key).getEncoded();

    // Compare the encoded (byte array) form of the keys - they must be
    // equal.
    Assert.assertArrayEquals(reference_key, unwrapped_secret_key);
  }
}
