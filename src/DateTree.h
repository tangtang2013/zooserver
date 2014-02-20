#ifndef _DATETREE_H_
#define _DATETREE_H_

#include "DateNode.h"
#include "PathTrie.h"

class DateTree
{
public:
	DateTree();
	~DateTree();

	bool isSpecialPath(string& path);
	void copyStatPersisted(StatPersisted* from, StatPersisted* to);
	//void copyStat(Stat* from, Stat* to);
	void updateCount(string& lastPrefix, int diff);
protected:
private:
	map<string,DateNode*> nodes;
	PathTrie* pTrie;
	DateNode* root;
	DateNode* procDateNode;
	map<long,set<string>> ephemerals;
};

#endif