#ifndef _WATCHER_H_
#define _WATCHER_H_

#include "WatcherEvent.h"

enum KeeperState {
	Unknown=-1,
	Disconnected=0,
	NoSyncConnected=1,
	SyncConnected=3,
	AuthFailed=4,
	ConnectedReadOnly=5,
	SaslAuthenticated=6,
	Expired=-112
};

enum EventType {
	None=-1,
	NodeCreated=1,
	NodeDeleted=2,
	NodeDataChanged=3,
	NodeChildrenChanged=4
};

class WatchedEvent{
public:
	WatchedEvent(EventType eventType, KeeperState keeperState, string& path){
		this->keeperState = keeperState;
        this->eventType = eventType;
        this->path = path;
	}
	WatchedEvent(WatchedEvent* eventMessage) {
        keeperState = eventMessage->getState();
        eventType = eventMessage->getType();
        path = eventMessage->getPath();
    }
    
    KeeperState getState() {
        return keeperState;
    }
    
    EventType getType() {
        return eventType;
    }
    
    string getPath() {
        return path;
    }

    string toString() {
		//return "WatchedEvent state:" + keeperState + " type:" + eventType + " path:" + path;
		return "WatchedEvent state";
    }

    WatcherEvent* getWrapper() {
        return new WatcherEvent(eventType, 
                                keeperState, 
                                path);
    }
private:
	KeeperState keeperState;
    EventType eventType;
    string path;
};

class Watcher{
public:
	void process(WatchedEvent event);
};

#endif