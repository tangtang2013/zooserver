#ifndef _INPUTARCHIVE_H_
#define _INPUTARCHIVE_H_

#include <string>
using namespace std;

class Record;

class Index
{
public:
	virtual bool done() = 0;
	virtual void incr() = 0;
	virtual void incr(int step) = 0;
	virtual int getIndex() = 0;
};

class InputArchive
{
public:
	virtual char readByte(string tag) = 0;
	virtual bool readBool(string tag) = 0;
	virtual int readInt(string tag) = 0;
	virtual long readLong(string tag) = 0;
	virtual float readFloat(string tag) = 0;
	virtual double readDouble(string tag) = 0;
	virtual string readString(string tag) = 0;
	virtual char* readBuffer(string tag) = 0;
	virtual void readRecord(Record r, string tag) = 0;
	virtual void startRecord(string tag) = 0;
	virtual void endRecord(string tag) = 0;
	virtual Index* startVector(string tag) = 0;
	virtual void endVector(string tag) = 0;
	virtual Index* startMap(string tag) = 0;
	virtual void endMap(string tag) = 0;
};

#endif