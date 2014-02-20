#ifndef _PATHTRIE_H_
#define _PATHTRIE_H_

#include "TrieNode.h"

class PathTrie{
public:
	PathTrie();
	~PathTrie();

	void addPath(string path);
	void deletePath(string path);
	string findMaxPrefix(string path);
private:
	TrieNode* rootNode;
};

#endif