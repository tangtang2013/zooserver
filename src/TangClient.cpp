#include <stdio.h>
#include "TangClient.h"
#include "..\net\anet.h"

#include <winsock2.h>  /* setsocketopt */
#include <errno.h>
#include "..\net\win32fixes.h"
#include "TangServer.h"
#include "Message.h"

TangClient::TangClient(struct aeEventLoop *eventLoop, int fd, TangServer* tangServer){
	el = eventLoop;
	clientfd = fd;
	server = tangServer;
	init();
}

TangClient::~TangClient(){

}

void TangClient::reInitUseParam(struct aeEventLoop *eventLoop, int fd, TangServer* tangServer){
	el = eventLoop;
	clientfd = fd;
	server = tangServer;
	init();
}

int TangClient::init(){
	if(anetNonBlock(err,clientfd) != ANET_OK){
		printf("err block:%s\n",err);
	}

	if(anetTcpNoDelay(err,clientfd) != ANET_OK){
		printf("err delay:%s\n",err);
	}

	if(aeCreateFileEvent(el, clientfd, AE_READABLE, readFileProc,this) != AE_OK){
		printf("err create file event fd[%d]\n",clientfd);
		return 1;
	}

	return 0;
}

void TangClient::cmdHandler(){
	PutRequestMessage* msg = new PutRequestMessage();
	send(clientfd, "hello world", 11, 0);
}

int TangClient::getSocketFd(){
	return clientfd;
}

void TangClient::close(){
	//关闭socket
	closesocket(clientfd);
	//取消对client的读监听
	aeDeleteFileEvent(el, clientfd, AE_READABLE);
	//将client连接加入到CloseClientMap
	server->putClientInCloseMap(this);
}

void TangClient::readFileProc(struct aeEventLoop *eventLoop, int sfd, void *clientData, int mask){
	int nread = 0;

	TangClient* tangClient = (TangClient*)clientData;

	//close...
	nread = recv(sfd, tangClient->buf, 1024, 0);
	if (nread < 0) {
        errno = WSAGetLastError();
		printf("client err : %d\n",errno);
        if (errno == WSAECONNRESET) {
            /* Windows fix: Not an error, intercept it.  */
            printf("Client closed connection[%d]\n",sfd);
			tangClient->close();
            return;
        } else if ((errno == ENOENT) || (errno == WSAEWOULDBLOCK)) {
            /* Windows fix: Intercept winsock slang for EAGAIN */
            errno = EAGAIN;
            nread = -1; /* Winsock can send ENOENT instead EAGAIN */
		} else if(errno == WSAECONNABORTED) {
			//TODO
			//free client
			printf("Client abort connection[%d]\n",sfd);
			tangClient->close();
			return;
		}

    }

	//win平台必须要这句话，否则会后续的read事件会监听异常
	aeWinReceiveDone(sfd);

	tangClient->cmdHandler();
}
