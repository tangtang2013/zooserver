#include "TangServer.h"
//#include "net\win32fixes.h"
//#include "net\anet.h"
#include "TangClient.h"
#include "../net\anet.h"

TangServer::TangServer(int port):port(port){
	addr = "127.0.0.1";
}

TangServer::~TangServer(){
	aeDeleteEventLoop(el);

}

int TangServer::init(){

	//重定义冲突
	//w32initWinSock();

	el = aeCreateEventLoop();

	serverfd = anetTcpServer(err,9999,(char*)addr.c_str());
	if(serverfd <= 0){
		printf("err:%s\n",err);
		return 1;
	}

	int ret = aeCreateFileEvent(el, serverfd, AE_READABLE, acceptProc, this);
	if(ret != AE_OK){
		printf("create file event err\n");
		return 1;
	}

	return 0;
}

void TangServer::start(){
	printf("TangServer ready\n");
	aeMain(el);
	//TODO
}

void TangServer::stop(){
	aeStop(el);
	//TODO
	std::map<int, TangClient*>::iterator it1 = closeclientmap.begin();
	for(;it1 != closeclientmap.end(); it1++){
		//it1->second->close();
		delete it1->second;
	}

	std::map<int, TangClient*>::iterator it2 = clientmap.begin();
	for(;it2 != closeclientmap.end(); it2++){
		it2->second->close();
		delete it2->second;
	}
}

TangClient* TangServer::createClient(struct aeEventLoop *eventLoop, int fd){
	TangClient* tangClient;
	
	//如果closeclientmap没有可用连接,创建一个新的client
	if(closeclientmap.size() == 0){
		tangClient = new TangClient(eventLoop, fd, this);
	} 
	//closeclientmap有可用连接，重新利用
	else {
		std::map<int, TangClient*>::iterator it = closeclientmap.begin();
		if(it != closeclientmap.end()){
			tangClient = it->second;
			tangClient->reInitUseParam(eventLoop, fd, this);
			closeclientmap.erase(it);
		}
	}

	//创建的client连接加入clientmap
	clientmap[fd] = tangClient;

	return tangClient;
}

void TangServer::closeClient(TangClient* pClient){
	if(pClient){
		pClient->close();
	}
}

void TangServer::putClientInCloseMap(TangClient* pClient){
	//在clientmap中删除client
	std::map<int, TangClient*>::iterator it = clientmap.find(pClient->getSocketFd());
	if(it != clientmap.end()){
		clientmap.erase(it);
	}

	//把client加入到closeclientmap
	closeclientmap[pClient->getSocketFd()] = pClient; 
}

void TangServer::acceptProc(struct aeEventLoop *eventLoop, int sfd, void *clientData, int mask){
	int afd = -1;
	char err[128];

	TangServer* tangServer = (TangServer*)clientData;
	
	if(sfd == tangServer->serverfd){
		afd = anetTcpAccept(err,sfd,NULL,NULL);
		if(afd <= 0){
			printf("accept err:%s\n",err);
			return;
		}

		TangClient* tangClient = tangServer->createClient(eventLoop, afd);
	}
	
	printf("accept ok fd[%d]\n",afd);
}