#include "PathTrie.h"

PathTrie::PathTrie(){
	rootNode = new TrieNode(NULL);
}

PathTrie::~PathTrie(){
	delete rootNode;
}

void PathTrie::addPath(string path){
	if(path == ""){
		return;
	}
	string part;
	TrieNode* parent = rootNode;
	int pos1=0,pos2=0,index=0;
	do{
		pos1=path.find_first_of("/",index);
		index = pos1 + 1;
		pos2=path.find_first_of("/",index);
		if (pos2 < 0)
		{
			printf("%d-end\n",pos1);
			part = path.substr(pos1 + 1);
			//parent->addChild(path.substr(pos1),new TrieNode(parent));
		}
		else
		{
			printf("%d-%d\n",pos1,pos2);
			part = path.substr(pos1 + 1,pos2-pos1-1);
			//parent->addChild(path.substr(pos1,pos2-pos1),new TrieNode(parent));
		}
		if (parent->getChild(part) == NULL)
		{
			parent->addChild(part,new TrieNode(parent));
		}
		parent = parent->getChild(part);
	}while(pos2>=0);
	parent->setProperty(true);
}

void PathTrie::deletePath(string path){
	if(path == ""){
		return;
	}
	string part;
	TrieNode* parent = rootNode;
	int pos1=0,pos2=0,index=0;
	do{
		pos1=path.find_first_of("/",index);
		index = pos1 + 1;
		pos2=path.find_first_of("/",index);
		if (pos2 < 0)
		{
			printf("%d-end\n",pos1);
			part = path.substr(pos1 + 1);
			//parent->addChild(path.substr(pos1),new TrieNode(parent));
			break;
		}
		else
		{
			printf("%d-%d\n",pos1,pos2);
			part = path.substr(pos1 + 1,pos2-pos1-1);
			//parent->addChild(path.substr(pos1,pos2-pos1),new TrieNode(parent));
		}
		if (parent->getChild(part) == NULL)
		{
			return;
		}
		parent = parent->getChild(part);
	}while(pos2>=0);
	//parent = parent->getParent();
	parent->delelteChild(part);
}

string PathTrie::findMaxPrefix(string path){
	if (path == ""){
		return "";
	}
	if (path == "/")
	{
	}

	string part;
	TrieNode* parent = rootNode;
	int pos1=0,pos2=0,index=0,des=0;
	do{
		pos1=path.find_first_of("/",index);
		index = pos1 + 1;
		pos2=path.find_first_of("/",index);
		if (pos2 < 0)
		{
			printf("%d-end\n",pos1);
			part = path.substr(pos1 + 1);
			//parent->addChild(path.substr(pos1),new TrieNode(parent));
		}
		else
		{
			printf("%d-%d\n",pos1,pos2);
			part = path.substr(pos1 + 1,pos2-pos1-1);
			//parent->addChild(path.substr(pos1,pos2-pos1),new TrieNode(parent));
		}
		if (parent->getChild(part) == NULL)
		{
			break;
		}
		else
		{
			des = pos1;
		}
		parent = parent->getChild(part);
	}while(pos2>=0);

	return path.substr(0,des);
}