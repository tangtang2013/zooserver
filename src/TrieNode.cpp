#include "TrieNode.h"

TrieNode::TrieNode(TrieNode* parent):property(false){
	this->parent = parent;
}

TrieNode::~TrieNode(){

}

void TrieNode::setParent(TrieNode* parent){
	this->parent = parent;
}

TrieNode* TrieNode::getParent(){
	return parent;
}

void TrieNode::setProperty(bool prop){
	this->property = prop;
}

bool TrieNode::getProperty(){
	return property;
}

set<string> TrieNode::getChildren(){
	set<string> nameset;
	map<string,TrieNode*>::iterator it = children.begin();
	while(it != children.end()){
		nameset.insert((*it).first);
	}
	return nameset;
}

int TrieNode::getSize(){
	return children.size();
}

void TrieNode::addChild(string& name, TrieNode* child){
	map<string,TrieNode*>::iterator find = children.find(name);
	if(find != children.end()){
		return;
	}
	else{
		children.insert(pair<string, TrieNode*>(name,child));
	}
}

TrieNode* TrieNode::getChild(string& name){
	map<string,TrieNode*>::iterator find = children.find(name);
	if(find == children.end()){
		return NULL;
	}
	else{
		return (*find).second;
	}
}

void TrieNode::delelteChild(string& name){
	map<string,TrieNode*>::iterator find = children.find(name);
	if(find == children.end()){
		return ;
	}

	TrieNode* node = (*find).second;
	if(node->getSize() == 1){
		node->setParent(NULL);
		children.erase(find);
	}
	else{
		node->setProperty(false);
	}
}