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
	send(clientfd, "hello world", 11, 0);
	//dynamic_cast

	Message* msg= NULL;
	int32_t magicid = 0;
	MessageType type;
	
	//��ȡMessage Type
	memcpy(&type, buf, sizeof(MessageType));

	//���magicid�Ƿ�ƥ��
	memcpy(&magicid, buf  + sizeof(MessageType), sizeof(int32_t));
	if(magicid != MAGIC_ID){
		return ;
	}
	
	switch(type){
	case PUTREQUESTMSG:
		msg = new PutRequestMessage(buf, bufpos);
		break;
	case PUTREPLYMSG:
		msg = new PutReplyMessage(buf, bufpos);
		break;
	case GETREQUESTMSG:
		msg = new GetRequestMessage(buf, bufpos);
		break;
	case GETREPLYMSG:
		msg = new GetReplyMessage(buf, bufpos);
		break;
	default:
		printf("Message Type Error : %d\n",type);
		break;
	}

	msg->deserialize();
}

int TangClient::getSocketFd(){
	return clientfd;
}

void TangClient::close(){
	//�ر�socket
	closesocket(clientfd);
	//ȡ����client�Ķ�����
	aeDeleteFileEvent(el, clientfd, AE_READABLE);
	//��client���Ӽ��뵽CloseClientMap
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

	//winƽ̨����Ҫ��仰������������read�¼�������쳣
	aeWinReceiveDone(sfd);
	tangClient->bufpos = nread;
	tangClient->cmdHandler();
}
