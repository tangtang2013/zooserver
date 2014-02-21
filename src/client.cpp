#include <stdio.h>
#include "..\net\anet.h"
#include "Message.h"
#include <WinSock2.h>

int initWinSock(void) {

    WSADATA t_wsa;
    WORD wVers;
    int iError;

    wVers = MAKEWORD(2, 2);
    iError = WSAStartup(wVers, &t_wsa);

    if(iError != NO_ERROR || LOBYTE(t_wsa.wVersion) != 2 || HIBYTE(t_wsa.wVersion) != 2 ) {
        return 0; /* not done; check WSAGetLastError() for error number */
    };

    return 1;
}

int main(){
	int fd;
	char err[128];
	char* addr="127.0.0.1";
	int port = 9999;
	initWinSock();
	fd = anetTcpConnect(err, addr, port);
	if(fd <= 0){
		printf("%s\n",err);
		return 1;
	}
	PutRequestMessage* msg = dynamic_cast<PutRequestMessage*>(Message::createMessage(PUTREQUESTMSG));
	msg->setKey("asd",3);
	msg->setValue("zxc",3);
	msg->serialize();
	int ret = anetWrite(fd,msg->getBuffer(),msg->getBufferSize());
	printf("ret : %d\n",ret);
	closesocket(fd);
}