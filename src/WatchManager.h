#ifndef _WATCHMANAGET_H_
#define _WATCHMANAGET_H_

#include <set>
#include <map>
#include "Watcher.h"
using namespace std;

class WatchManager{
public:
	int size(){
		int result = 0;
		map<string, set<Watcher*>>::iterator it = watchTable.begin();
		for(;it!=watchTable.end() ; ++it){
			result += (*it).second.size();
		}
		return result;
	}
	void addWatch(string& path, Watcher* watcher){
		set<Watcher*> list1;
		map<string, set<Watcher*>>::iterator it1 = watchTable.find(path);
		if(it1 == watchTable.end()){
			list1.insert(watcher);
			watchTable.insert(pair<string, set<Watcher*>>(path, list1));
		}
		else{
			(*it1).second.insert(watcher);
		}

		set<string> list2;
		map<Watcher*, set<string>>::iterator it2 = watch2Paths.find(watcher);
		if(it2 == watch2Paths.end()){
			list2.insert(path);
			watch2Paths.insert(pair<Watcher*, set<string>>(watcher, list2));
		}
		else{
			(*it2).second.insert(path);
		}
	}
	void removeWatcher(Watcher* watcher){
		map<Watcher*, set<string>>::iterator it1 = watch2Paths.find(watcher);
		if(it1 == watch2Paths.end()){
			return;
		}
		set<string>::iterator it2 = (*it1).second.begin();
		for(;it2 != (*it1).second.end() ; ++it2){

		}
		watch2Paths.erase(it1);
	}
private:
	map<string, set<Watcher*>> watchTable;
	map<Watcher*, set<string>> watch2Paths;
};

#endif