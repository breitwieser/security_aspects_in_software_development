//
// Example for KLEE
//
#include <stdio.h>

#include <klee/klee.h>

//----------------------------------------------------------------------
int hexdigit(char c)
{
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'a' && c <= 'f') {
 return c - 'a';
  } else if (c >= 'A' && c <= 'F') {
 return c - 'A';
  } else {
    return -1;
  }
}

//----------------------------------------------------------------------
// A simple "urldecode" function with several bugs.
//
void urldecode(char *dst, const char *src)
{
  while (*src != '\0') {
    char c = *src++;

    if (c == '%') {
      // Got a '%' - need to decode hex
      int hi = hexdigit(*src++);
      int lo = hexdigit(*src++);
      if (hi != -1 || lo != -1) {
        char o = (char)((hi << 4) | lo);
        *dst++ = o;
      }

    } else if (c == '+') {
        // Got a '+' - decode as space
        *dst++ = ' ';

      } else {
      // No special handling required
      *dst++ = c;
    }
  }

  // Write the trailing '\0'
  *dst++ = '\0';
}

//----------------------------------------------------------------------
int main(void)
{
  // URL length for testing (note that larger values greatly increase the
  // runtime)
#define URL_LEN 6

  // The input URL
  char input[URL_LEN];

  /// \todo Make your URL "input" variable symbolic and find a way to tell
  ///  KLEE that it is a valid C string, which always ends with '\0'.

  // The output URL
  // (we intentionally make this buffer a little bit too small, to cause some
  //  errors for KLEE to find)
  char output[URL_LEN - 1];

  // Now run our decode function under test
  urldecode(output, input);

  return 0;
}
