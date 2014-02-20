#ifndef _QUOTAS_H_
#define _QUOTAS_H_

class Quotas
{
public:
	static string procZookeeper;
	static string quotaZookeeper;
	static string limitNode;
	static string statNode;

	static string quotaPath(string& path){
		return quotaZookeeper + path + "/" + limitNode;
	}
	static string statPath(string& path){
		return quotaZookeeper + path + "/" + statNode;
	}
};

string Quotas::procZookeeper = "/zookeeper";
string Quotas::quotaZookeeper = "/zookeeper/quota";
string Quotas::limitNode = "zookeeper_limits";
string Quotas::statNode = "zookeeper_stats";

#endif