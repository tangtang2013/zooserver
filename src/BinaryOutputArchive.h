#ifndef _BINARYOUTPUTARCHIVE_H_
#define _BINARYOUTPUTARCHIVE_H_

#include <stdint.h>
#include "OutputArchive.h"

#define BUFFER_MAX_LEN 1024*1024

class BinaryOutputArchive
{
public:
	BinaryOutputArchive(){
		buffersize = 32;
		size = 0;
		out = (char*)malloc(32);
	}

	~BinaryOutputArchive(){
		free(out);
	}

	char* getBuffer(){
		return out;
	}

	int getSize(){
		return size;
	}

	void writeByte(char c){
		int len = sizeof(c);
		if(expend(len)){
			//memcpy(out+size, &c, len);
			out[size] = c;
			size += len;
		}
		else
		{
			//exception...
		}
	}
	void writeBool(bool b){
		if(b){
			writeByte(1);
		}
		else
		{
			writeByte(0);
		}
	}
	void writeInt32(int32_t i){
		int len = sizeof(i);
		if(expend(len)){
			memcpy(out+size, &i, len);
			size += len;
		}
		else
		{
			//exception...
		}
	}
	void writeInt64(int64_t i){
		int len = sizeof(i);
		if(expend(len)){
			memcpy(out+size, &i, len);
			size += len;
		}
		else
		{
			//exception...
		}
	}
	void writeLong(long l){
		int len = sizeof(l);
		if(expend(len)){
			memcpy(out+size, &l, len);
			size += len;
		}
		else
		{
			//exception...
		}
	}
	void writeFloat(float f){
		int len = sizeof(f);
		if(expend(len)){
			memcpy(out+size, &f, len);
			size += len;
		}
		else
		{
			//exception...
		}
	}
	void writeDouble(double d){
		int len = sizeof(d);
		if(expend(len)){
			memcpy(out+size, &d, len);
			size += len;
		}
		else
		{
			//exception...
		}
	}
	void writeString(char* str,  int strlen){
		if(expend(strlen)){
			memcpy(out+size, str, strlen);
			size += strlen;
		}
		else
		{
			//exception...
		}
	}
protected:
private:

	bool expend(int writesize){
		if(size + writesize > buffersize){
			if(size + writesize > BUFFER_MAX_LEN){
				return false;
			}
			else{
				buffersize *= 2;
				out = (char*)realloc(out, buffersize);
				return true;
			}
		}
		return true;
	}
	char* out;
	int buffersize;
	int size;
};

#endif