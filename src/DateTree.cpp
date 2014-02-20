#include "DateTree.h"
#include "Quotas.h"

DateTree::DateTree()
{
	root = new DateNode(NULL,NULL,-1L,new StatPersisted());
	procDateNode = new DateNode(root,NULL,-1L,new StatPersisted());
	nodes.insert(pair<string,DateNode*>("",root));
	nodes.insert(pair<string,DateNode*>("/",root));

	root->addChild(string("zookeeper"));
	nodes.insert(pair<string,DateNode*>("/zookeeper",procDateNode));
}

DateTree::~DateTree()
{

}

bool DateTree::isSpecialPath( string& path )
{
	if (path == "/" || path == "/zookeeper" || path == "/zookeeper/quota")
	{
		return true;
	}
	return false;
}

void DateTree::copyStatPersisted( StatPersisted* from, StatPersisted* to )
{
	to->setVersion(from->getVersion());
	to->setAversion(from->getAversion());
	to->setCversion(from->getCversion());
	to->setCtime(from->getCtime());
	to->setMtime(from->getMtime());
	to->setCzxid(from->getCzxid());
	to->setMzxid(from->getMzxid());
	to->setPzxid(from->getMzxid());
	to->setEphemeralOwner(from->getEphemeralOwner());
}

void DateTree::updateCount( string& lastPrefix, int diff )
{
	string statNode = Quotas::statPath(lastPrefix);
	map<string,DateNode*>::iterator find = nodes.find(statNode);
	if (find == nodes.end())
	{
		printf("Missing count node for stat : %s\n", statNode.c_str());
		return;
	}
}

