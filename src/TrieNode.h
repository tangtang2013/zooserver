#ifndef _TRIENODE_H_
#define _TRIENODE_H_

#include <map>
#include <set>
#include <string>
using namespace std;

class TrieNode{
public:
	TrieNode(TrieNode* parent);
	~TrieNode();

	void setParent(TrieNode* parent);
	TrieNode* getParent();

	void setProperty(bool prop);
	bool getProperty();

	set<string> getChildren();
	int getSize();

	void addChild(string& name, TrieNode* child);
	TrieNode* getChild(string& name);
	void delelteChild(string& name);
private:
	bool property;
	TrieNode* parent;
	map<string, TrieNode*> children;
};

#endif