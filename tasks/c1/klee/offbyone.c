//
// Simple off-by-one bug.
//
#include <stdio.h>

#include <klee/klee.h>

//----------------------------------------------------------------------
void StringCopy(char *dst, const char *src)
{
  while (*src != '\0') {
    *dst++ = *src++;
  }

  *dst++ = '\0';
}

//----------------------------------------------------------------------
void StringCat(char *dst, const char *src)
{
  while (*dst != '\0') {
    dst += 1;
  };

  StringCopy(dst, src);
}

//----------------------------------------------------------------------
void HelloWorld(const char *name)
{
  char message[64];
  StringCopy(message, "Hello, ");
  StringCat(message, name);
  StringCat(message, "!");
  puts(message);
}

//----------------------------------------------------------------------
int main(void)
{
  /// \todo Write a text fixture for HelloWorld, which allows KLEE
  ///   to detect the buffer overflows.
  //
  ///  HINT: Declare a "name" variable as suitable fixed-size character
  ///  arrray and use klee_make_symbolic to mark it as symbolic value.
  ///  Then call HelloWorld with your symbolic variable as argument.
  return 0;
}
