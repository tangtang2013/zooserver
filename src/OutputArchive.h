#ifndef _OUTPUTARCHIVE_H_
#define _OUTPUTARCHIVE_H_

#include <string>
using namespace std;

class Record;

class OutputArchive
{
public:
	virtual void writeByte(char b, string tag) = 0;
	virtual void writeBool(bool b, string tag) = 0;
	virtual void writeInt(int i, string tag) = 0;
	virtual void writeLong(long l, string tag) = 0;
	virtual void writeFloat(float f, string tag) = 0;
	virtual void writeDouble(double d, string tag) = 0;
	virtual void writeString(string& s, string tag) = 0;
	virtual void writeBuffer(char buf[], string tag) = 0;
	virtual void writeRecord(Record r, string tag) = 0;
	virtual void startRecord(Record r, string tag) = 0;
	virtual void endRecord(Record r, string tag) = 0;
	//void startVector(List v, String tag);
	//void endVector(List v, String tag);
	//void startMap(TreeMap v, String tag);
	//void endMap(TreeMap v, String tag);
};

#endif