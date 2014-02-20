#ifndef _BINARYINPUTARCHIVE_H_
#define _BINARYINPUTARCHIVE_H_

#include "coding.h"
#include "InputArchive.h"

class BinaryInputArchive// : public InputArchive
{
public:
	BinaryInputArchive(char* buf, int size){
		in = buf;
		index = new BinaryIndex(size);
	}

	~BinaryInputArchive(){
		delete index;
	}

	class BinaryIndex : public Index{
	public:
		BinaryIndex(int nelems){
			this->nelems = nelems;
			index = 0;
		}
		bool done(){
			return (nelems <= 0);
		}
		void incr(){
			--nelems;
			++index;
		}
		void incr(int step){
			nelems-=step;
			index+=step;
		}
		int getIndex(){
			return index;
		}
	private:
		int nelems;
		int index;
	};


	void readByte(char& c){
		if(!index->done())
		{
			//memcpy(&c, in + index->getIndex(), sizeof(c));
			c = in[index->getIndex()];
			index->incr();
		}
		else
		{
			//exception...
		}
	}
	void readBool(bool& b){
		if(!index->done())
		{
			uint8_t c;
			memcpy(&c, in + index->getIndex(), sizeof(b));
			index->incr();
			if(c){
				b = true;
			}
			else{
				b = false;
			}
		}
		else
		{
			//exception...
		}
	}
	void readInt32(int32_t& i){
		if(!index->done())
		{
			// Load the raw bytes
			memcpy(&i, in + index->getIndex(), sizeof(i));  // gcc optimizes this to a plain load
			index->incr(sizeof(i));
		}
		else
		{
		}
	}
	void readInt64(int64_t& i){
		if(!index->done())
		{
			// Load the raw bytes
			memcpy(&i, in + index->getIndex(), sizeof(i));  // gcc optimizes this to a plain load
			index->incr(sizeof(i));
		}
		else
		{
		}
	}
	void readLong(long& l){
		if(!index->done())
		{
			// Load the raw bytes
			memcpy(&l, in + index->getIndex(), sizeof(l));  // gcc optimizes this to a plain load
			index->incr(sizeof(l));
		}
		else
		{
		}
	}
	void readFloat(float& f){
		if(!index->done())
		{
			// Load the raw bytes
			memcpy(&f, in + index->getIndex(), sizeof(f));  // gcc optimizes this to a plain load
			index->incr(sizeof(f));
		}
		else
		{
		}
	}
	void readDouble(double& d){
		if(!index->done())
		{
			// Load the raw bytes
			memcpy(&d, in + index->getIndex(), sizeof(d));  // gcc optimizes this to a plain load
			index->incr(sizeof(d));
		}
		else
		{
		}
	}
	void readString(char* out,  int size){
		memcpy(out, in + index->getIndex(), size);
		//out.append(in + index->getIndex(), size);
		index->incr(size);
	}
protected:
private:
	char* in;
	Index* index;
};

#endif