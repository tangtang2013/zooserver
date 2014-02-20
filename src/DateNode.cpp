#include "DateNode.h"

DateNode::DateNode(){

}

DateNode::DateNode(DateNode* parent, char* data, long acl, StatPersisted* stat){
	this->parent = parent;
	this->data = data;
	this->acl = acl;
	this->stat = stat;
}

DateNode::~DateNode(){

}

bool DateNode::addChild(string& child){
	children.insert(child);
	return true;
}

bool DateNode::removeChild(string& child){
	set<string>::iterator find = children.find(child);
	if (find == children.end())
	{
		return false;
	} 
	else
	{
		children.erase(find);
		return true;
	}
}

void DateNode::setChildren(set<string>& children){
	this->children = children;
}

set<string>& DateNode::getChildren(){
	return children;
}

void DateNode::deserialize(InputArchive* archive, string tag){
	archive->startRecord("node");
	data = archive->readBuffer("data");
	acl = archive->readLong("acl");
	stat = new StatPersisted();
	stat->deserialize(archive,"statpersisted");
	archive->endRecord("node");
}

void DateNode::serialize(OutputArchive* archive, string tag){
	archive->startRecord(*this,"node");
	archive->writeBuffer(data,"data");
	archive->writeLong(acl,"acl");
	stat->serialize(archive,"statpersisted");
	archive->endRecord(*this,"node");
}