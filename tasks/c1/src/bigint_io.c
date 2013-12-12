///
/// \file
/// \brief Big integer input and output
///
#include "tinybn_imp.h"

//----------------------------------------------------------------------
size_t BigIntGetWordCount(const BigInteger *z)
{
  /// \todo Returns the number of words used in the word array of
  ///   your big integer.

  if(z == NULL)
    return 0;
  if(z->words == NULL)
    return 0;
  return z->wordcount;
}

//----------------------------------------------------------------------
mp_word_t BigIntGetAt(const BigInteger *z, size_t index)
{
  /// \todo Return the word at the given inex from the big integer's
  ///   word array. The word containing the least significant bit is
  ///   word at index 0. Trying to access a non-existing word (out of
  ///   bounds) returns zero.

  if(z == NULL)
    return 0;
  if(z->words == NULL)
    return 0;
  if(index < 0 || index >= z->wordcount)
    return 0;
  return z->words[index];
}

//----------------------------------------------------------------------
bool BigIntSetAt(BigInteger *z, size_t index, mp_word_t value)
{
  /// \todo Sets a given word of the big integer's word array to the
  ///   given value. The word containing the least significant bit is
  ///   word at index 0. The word array is grown by this function if
  ///   the user specifies a non-existing index.
  if(z == NULL)
    return false;
  if(z->words == NULL)
    return false;
  if(index < 0)
    return false;
  if(index >= z->wordcount)
  {
    if(value == 0)
      return true;
    z->words = realloc(z->words, index - (z->wordcount-1));
    if(z->words == NULL)
      return false;
    z->wordcount = index+1;
  }
  else if(index == z->wordcount-1 && value == 0)
  {
    z->words = realloc(z->words, z->wordcount-1);
    if(z->words == NULL)
      return false;
    z->wordcount--;
    return true;
  }
  z->words[index] = value;
  return true;
}

//----------------------------------------------------------------------
bool BigIntLoad(BigInteger *z, const unsigned char *data, size_t len)
{
  /// \todo Load the byte array into the big integer.
  if(z == NULL)
    return false;
  if(z->words == NULL)
    return false;
  if(len == 0)
  {
    BigIntFree(z);
    z = BigIntAlloc();
    return (z != NULL);
  }
  if(data == NULL)
    return false;
  size_t usedlen = len;

  for(size_t i = 0; i < usedlen; i++)
  {
      if(data[i] != 0)
        {
          usedlen -= i;
          break;
        }
  }
  size_t count = (int)(usedlen/4.0);
  size_t remain = usedlen%4;
  z->sign = positive; // todo check for 0
  size_t mallcount = remain == 0 ? count :count+1;
  z->wordcount = mallcount;
  z->words = realloc(z->words, mallcount*sizeof(mp_word_t));
  if(z->words == NULL)
    return NULL;
  for(size_t i = 0; i < count; i++)
  {
      mp_word_t word = data[len-(i*4)-1]       | data[len-(i*4)-2] << 8 |
                       data[len-(i*4)-3] << 16 | data[len-(i*4)-4] << 24;
      z->words[i] = word;
  }
  if(remain != 0)
  {
    mp_word_t last = 0;
    for(size_t i = 0; i < remain; i++)
      last |= data[i] << 8*i;
    z->words[count] = last;
  }
  return true;
}

//----------------------------------------------------------------------
bool BigIntSave(unsigned char *data, size_t len, const BigInteger *a)
{
  /// \todo Save the big integer into a byte array.  
  if(a == NULL)
    return false;
  if(len == 0)
    return true;
  if(data == NULL || a->words == NULL)
    return false;
  size_t wordcount = a->wordcount;
  size_t charcount = (len/4);
  size_t remain = len%4;

  if(charcount >= wordcount)
  {
    for(size_t i = 0; i < wordcount; i++)
    {
        data[len-(i*4)-1] = a->words[i] & 255;
        data[len-(i*4)-2] = (a->words[i] & 65280) >> 8;
        data[len-(i*4)-3] = (a->words[i] & 16711680) >> 16;
        data[len-(i*4)-4] = (a->words[i] & 4278190080) >> 24;
    }
  }
  else {
      size_t i = 0;
      for(; i < charcount; i++)
      {
          data[len-(i*4)-1] = a->words[i] & 255;
          data[len-(i*4)-2] = (a->words[i] & 65280) >> 8;
          data[len-(i*4)-3] = (a->words[i] & 16711680) >> 16;
          data[len-(i*4)-4] = (a->words[i] & 4278190080) >> 24;
      }
      if(remain != 0)
      {
        for(size_t j = i; j < remain+i; j++)
        {
            mp_word_t dfs = (255 << (8*(j-i)));
            mp_word_t df = (a->words[j] & dfs);
            data[len-j-1] = df >> (8*(j-i));
        }
      }
  }

  return true;
}
