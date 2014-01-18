///
///
/// \file
/// \brief Implementation of the byte buffer.
///
#include "buffer_imp.h"

//----------------------------------------------------------------------
Buffer* BufferCreate(void)
{
  /// \todo Add your own implementation
	Buffer *b = (Buffer*) malloc(sizeof(Buffer));
	if(b == NULL)
	{
		return NULL;
	}
	b->data=NULL;
	b->len=0;
	return b;
}

//----------------------------------------------------------------------
void BufferFree(Buffer* buffer)
{
  /// \todo Add your own implementation
	if(buffer != NULL)
	{
		if(buffer->data != NULL)
		{
			free(buffer->data);
			buffer->data=NULL;
		}
		free(buffer);
	}
}

//----------------------------------------------------------------------
bool BufferAppend(Buffer* buffer, const void *data, size_t len)
{
  /// \todo Add your own implementation here. Be aware that data/len may
  ///  refers to a (sub)slice of the buffer given in buffer ....
	if(buffer == NULL)
		return false;
	if(len == 0)
		return true;
	if(data == NULL)
		return false;


	unsigned char *c_data = (unsigned char*) data;


	// is data subslice of buffer?
	size_t offset=0;
	if (buffer->data != NULL) {
		unsigned char *buf_last_el = buffer->data + buffer->len - 1;
		//overflow
		if (buf_last_el < buffer->data)
			return false;
		if (buffer->data <= c_data && buf_last_el >= c_data) {
			//data start pointer lies within buffer boundaries
			//check if data+len is inside the buffer
			unsigned char *data_last_el = c_data + len - 1;
			// overflow?
			if (data_last_el < c_data)
				return false;
			if (buf_last_el < data_last_el) {
				//data starts within buffer but exceeds buffer boundaries
				return false;
			}
			offset = c_data - buffer->data;
		}
	}


	size_t new_len = buffer->len+len;
	// size_t overflow?
	if(new_len < len)
		return false;
	unsigned char *tmp = (unsigned char*) realloc(buffer->data, new_len * sizeof(unsigned char));
	if(tmp == NULL)
	{
		return false;
	}
	buffer->data = tmp;
	unsigned char *start = buffer->data+buffer->len;
	buffer->len=new_len;

	//if data is subslice of buffer
	if(offset != 0)
	{
		//we don't use c_data here, because data block could be moved
		//to an arbitrary place by calling realloc. this would invalidate c_data
		memcpy(start, buffer->data+offset, len);
	}
	else
	{
		memcpy(start, c_data, len);
	}
	return true;

}

//----------------------------------------------------------------------
bool BufferSkip(Buffer* buffer, unsigned char filler, size_t len)
{
  /// \todo Add your own implementation here. Be aware that data/len may
  ///  refers to a (sub)slice of the buffer given in buffer ....
	if(buffer ==NULL)
		return false;

	//create data
	unsigned char *data = (unsigned char*) malloc(len);
	if(data == NULL)
		return false;
	for(size_t i=0; i<len;i++)
	{
		data[i] = filler;
	}
	bool ret = BufferAppend(buffer, data, len);
	free(data);
	return ret;
}

//----------------------------------------------------------------------
bool BufferClear(Buffer* buffer)
{
	if(buffer == NULL)
		return false;

	if(buffer->data)
	{
		free(buffer->data);
		buffer->data=NULL;
	}
	buffer->len=0;
	return true;
}

//----------------------------------------------------------------------
unsigned char* BufferGetBytes(Buffer* buffer, size_t offset, size_t len)
{
  /// \todo Add your own implementation here.
	if(buffer == NULL)
		return NULL;
	if(len == 0)
		return NULL;
	if(buffer->data == NULL)
		return NULL;

	//does offset + len overflow?
	if(offset > offset + len)
		return NULL;
	if (offset <= buffer->len - 1 && (buffer->len - 1) >= (offset + len - 1)) {
		return buffer->data + offset;
	}

  return NULL;
}

//----------------------------------------------------------------------
size_t BufferGetLength(const Buffer* buffer)
{
  /// \todo Add your own implementation here.
	if(buffer != NULL)
		return buffer->len;
  return 0;
}
