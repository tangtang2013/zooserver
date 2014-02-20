#include "StatPersisted.h"

StatPersisted::StatPersisted(){
}

StatPersisted::StatPersisted(long czxid,
					long mzxid,
					long ctime,
					long mtime,
					int version,
					int cversion,
					int aversion,
					long ephemeralOwner,
					long pzxid){
	this->czxid=czxid;
    this->mzxid=mzxid;
    this->ctime=ctime;
    this->mtime=mtime;
    this->version=version;
    this->cversion=cversion;
    this->aversion=aversion;
    this->ephemeralOwner=ephemeralOwner;
    this->pzxid=pzxid;
}

int StatPersisted::hashCode(){
	int result = 17;
// 	int ret;
// 	ret = (int) (czxid^(czxid>>32));
// 	result = 37*result + ret;
// 	ret = (int) (mzxid^(mzxid>>32));
// 	result = 37*result + ret;
// 	ret = (int) (ctime^(ctime>>32));
// 	result = 37*result + ret;
// 	ret = (int) (mtime^(mtime>>32));
// 	result = 37*result + ret;
// 	ret = (int)version;
// 	result = 37*result + ret;
// 	ret = (int)cversion;
// 	result = 37*result + ret;
// 	ret = (int)aversion;
// 	result = 37*result + ret;
// 	ret = (int) (ephemeralOwner^(ephemeralOwner>>32));
// 	result = 37*result + ret;
// 	ret = (int) (pzxid^(pzxid>>32));
// 	result = 37*result + ret;
	return result;
}

void StatPersisted::serialize( OutputArchive* archive, string tag )
{
	archive->writeRecord(*this,tag);
	archive->writeLong(czxid,"czxid");
	archive->writeLong(mzxid,"mzxid");
	archive->writeLong(ctime,"ctime");
	archive->writeLong(mtime,"mtime");
	archive->writeInt(version,"version");
	archive->writeInt(cversion,"cversion");
	archive->writeInt(aversion,"aversion");
	archive->writeLong(ephemeralOwner,"ephemeralOwner");
	archive->writeLong(pzxid,"pzxid");
	archive->endRecord(*this,tag);
}

void StatPersisted::deserialize( InputArchive* archive, string tag )
{
	archive->startRecord(tag);
	czxid = archive->readLong("czxid");
	mzxid = archive->readLong("mzxid");
	ctime = archive->readLong("ctime");
	mtime = archive->readLong("mtime");
	version = archive->readInt("version");
	cversion = archive->readInt("cversion");
	aversion = archive->readInt("aversion");
	ephemeralOwner=archive->readLong("ephemeralOwner");
	pzxid=archive->readLong("pzxid");
	archive->endRecord(tag);
}
