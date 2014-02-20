#ifndef _DATENODE_H_
#define _DATENODE_H_

#include <set>
#include "StatPersisted.h"

class DateNode : public Record
{
public:
	DateNode();
	DateNode(DateNode* parent, char* data, long acl, StatPersisted* stat);
	~DateNode();

	bool addChild(string& child);
	bool removeChild(string& child);
	void setChildren(set<string>& children);
	set<string>& getChildren();

	//void copyStat(stat to);

	void deserialize(InputArchive* archive, string tag);
	void serialize(OutputArchive* archive, string tag);

protected:
private:
	DateNode* parent;	//the parent of this datanode
	char* data;			//the data for this datanode
	long acl;			//the acl map long for this datanode.the datatree has the map
	StatPersisted* stat;
	set<string> children;
};

#endif