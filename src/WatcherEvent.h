#ifndef _WATCHEREVENT_H_
#define _WATCHEREVENT_H_

#include <string>
#include "Record.h"
using namespace std;

class WatcherEvent : public Record{
public:
	WatcherEvent(int type, int state, string& path) {
		this->type=type;
		this->state=state;
		this->path=path;
	}
	int getType() {
		return type;
	}
	void setType(int m_) {
		type=m_;
	}
	int getState() {
		return state;
	}
	void setState(int m_) {
		state=m_;
	}
	string getPath() {
		return path;
	}
	void setPath(string& m_) {
		path=m_;
	}
	int hashCode() {
		int result = 17;
		int ret;
		ret = (int)type;
		result = 37*result + ret;
		ret = (int)state;
		result = 37*result + ret;
		//ret = path.hashCode();
		result = 37*result + ret;
		return result;
	}
	static string signature() {
		return "LWatcherEvent(iis)";
	}
private:
	int type;
	int state;
	string path;
};

#endif