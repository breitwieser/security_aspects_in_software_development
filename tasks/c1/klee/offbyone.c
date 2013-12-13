//
// Simple off-by-one bug.
//
#include <stdio.h>

#include <klee/klee.h>

//----------------------------------------------------------------------
//void StringCopy(char *dst, const char *src)
void StringCopy(char *dst, size_t len, const char *src)
{
  //while (*src != '\0') {
  size_t counter = 0;
  while(*src != '\0' && counter < len){
    *dst++ = *src++;
    counter++;
  }
  *dst++ = '\0';
}

//----------------------------------------------------------------------
//void StringCat(char *dst, const char *src)
void StringCat(char *dst, size_t len, const char *src)
{
//  while (*dst != '\0') {
//    dst += 1;
//  };
//
//  StringCopy(dst, 64, src);
	size_t counter = 0;
	  while (*dst != '\0') {
	    dst += 1;
	    counter++;
	  };

	  StringCopy(dst, len-counter, src);
}

//----------------------------------------------------------------------
void HelloWorld(const char *name)
{
  char message[64];
  StringCopy(message,64, "Hello, ");
  StringCat(message, 64, name);
  StringCat(message, 64, "!");
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
#define L 55
  char name[L] = "SASE_SASE_SASE_SASE_SASE_SASE_SASE_SASE_SASE_SASE_SASE";
  klee_make_symbolic(name, sizeof(name), "name");
  klee_assume(name[L-1] == '\0');
  puts(name);
  HelloWorld(name);

	return 0;
}
