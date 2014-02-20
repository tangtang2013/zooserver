#ifndef _STATPERSISTED_H_
#define _STATPERSISTED_H_

#include "Record.h"

class StatPersisted : public Record
{
public:
	StatPersisted();
	StatPersisted(long czxid,
					long mzxid,
					long ctime,
					long mtime,
					int version,
					int cversion,
					int aversion,
					long ephemeralOwner,
					long pzxid);
	
	long getCzxid(){
		return czxid;
	}
	void setCzxid(long m_){
		czxid = m_;
	}

	long getMzxid(){
		return mzxid;
	}
	void setMzxid(long m_){
		mzxid = m_;
	}

	long getCtime(){
		return ctime;
	}
	void setCtime(long m_){
		ctime = m_;
	}

	long getMtime(){
		return mtime;
	}
	void setMtime(long m_){
		mtime = m_;
	}

	int getVersion(){
		return version;
	}
	void setVersion(long m_){
		version = m_;
	}

	int getCversion(){
		return cversion;
	}
	void setCversion(long m_){
		cversion = m_;
	}

	int getAversion(){
		return aversion;
	}
	void setAversion(long m_){
		aversion = m_;
	}

	long getEphemeralOwner(){
		return ephemeralOwner;
	}
	void setEphemeralOwner(long m_){
		ephemeralOwner = m_;
	}

	long getPzxid(){
		return pzxid;
	}
	void setPzxid(long m_){
		pzxid = m_;
	}

	void serialize(OutputArchive* archive, string tag);
	void deserialize(InputArchive* archive, string tag);
	string toString();

	void write();
	void readFields();
	int compareTo();
	bool equals();
	int hashCode();
protected:
private:
	long czxid;
	long mzxid;
	long ctime;
	long mtime;
	int version;
	int cversion;
	int aversion;
	long ephemeralOwner;
	long pzxid;
};

#endif