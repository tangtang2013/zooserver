#include <stdio.h>
#include <signal.h>
//#include "PathTrie.h"
//#include "WatchManager.h"
#include "TangServer.h"
#include <WinSock2.h>

TangServer* server;

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

void signalHandler(int signal){
	server->stop();
}

int main(){
	signal(SIGINT,signalHandler);
	signal(SIGTERM,signalHandler);
	initWinSock();
	server = new TangServer(9999);
	server->init();
	server->start();
	printf("TangServer stop\n");
	delete server;
}

#if 0
int sfd;
aeEventLoop* loop;

void readFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask){
    
    char buf[1024];
    int nread;

	memset(buf,0,1024);

#ifdef _WIN32
    nread = recv((SOCKET)fd, buf, 1024, 0);
    if (nread < 0) {
        errno = WSAGetLastError();
        if (errno == WSAECONNRESET) {
            /* Windows fix: Not an error, intercept it.  */
            printf("Client closed connection[%d]\n",fd);
            closesocket(fd);
			aeDeleteFileEvent(loop,fd,AE_READABLE);
            return;
        } else if ((errno == ENOENT) || (errno == WSAEWOULDBLOCK)) {
            /* Windows fix: Intercept winsock slang for EAGAIN */
            errno = EAGAIN;
            nread = -1; /* Winsock can send ENOENT instead EAGAIN */
        }
    }
#else
    nread = read(fd, buf, REDIS_IOBUF_LEN);
#endif
    if (nread == -1) {
        if (errno == EAGAIN) {
            nread = 0;
        } else {
            printf("Reading from client: %s",strerror(errno));
            closesocket(fd);
			aeDeleteFileEvent(loop,fd,AE_READABLE);
            return;
        }
    } else if (nread == 0) {
        printf("Client closed connection");
        closesocket(fd);
		aeDeleteFileEvent(loop,fd,AE_READABLE);
        return;
    }
#ifdef _WIN32
    aeWinReceiveDone(fd);
#endif

	char* aaa;
    printf("read:%s\n",buf);
	anetWrite(fd,"HTTP/1.1 200 OK",strlen("HTTP/1.1 200 OK"));
	anetWrite(fd,"Content-Length: 19",strlen("Content-Length: 19"));
	anetWrite(fd, "<html>", 6);
	anetWrite(fd, "hello", 6);
	anetWrite(fd, "<\html>", 7);
	
	shutdown(fd,0);
	return;

	if(strcmp(buf,"quit") == 0){
		closesocket(fd);
		aeDeleteFileEvent(loop,fd,AE_READABLE);
	}
}

void acceptProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask){
	int afd = -1;
    char err[128];
    if(fd == sfd){
        afd = anetTcpAccept(err,fd,NULL,NULL);
        if(afd <= 0){
            printf("err:%s\n",err);
            return;//----------------alarm
        }
		if(anetNonBlock(err,afd) != 0){
			printf("err block:%s\n",err);
		}
		anetTcpNoDelay(NULL,fd);
        aeCreateFileEvent(loop,afd,AE_READABLE,readFileProc,NULL);
        printf("accept\n");
    }
	printf("ok\n");
}

int main(int argc, char** argv){
	printf("zoo ...\n");
	PathTrie* tree = new PathTrie();
	string path = "/home/juntang/trunk/test";
	tree->addPath(path);
	tree->deletePath("/home/juntang/trunk/test");
	printf("prefix : %s\n",tree->findMaxPrefix("homg/trunk/test").c_str());
	WatchManager m;
	m.size();
	Watcher* w1 = new Watcher();
	Watcher* w2 = new Watcher();
	m.addWatch(path,w1);
	m.addWatch(path,w2);

	char err[128];
    loop = aeCreateEventLoop();

	w32initWinSock();
    sfd = anetTcpServer(NULL,9999,"127.0.0.1");
    if(sfd <= 0){
        printf("err:%s\n",err);
        return 1;
    }
    
    aeCreateFileEvent(loop,sfd,AE_READABLE,acceptProc,NULL);
    aeMain(loop);
}
#endif

#if 0

#include "BinaryInputArchive.h"
#include "BinaryOutputArchive.h"

int main(){
	BinaryOutputArchive out;
	out.writeString("hello world",11);
	out.writeString("hello world",11);
	out.writeString("hello world",11);
	bool b = false;
	out.writeBool(b);
	b = true;
	out.writeBool(b);
	out.writeByte('A');
	out.writeByte('z');
	out.writeDouble(3.14);
	out.writeDouble(-3.14);
	out.writeFloat(3.14);
	out.writeFloat(-3.14);
	out.writeInt32(65536);
	out.writeInt32(-65536);
	out.writeInt64(111111111111111);

	string str;
	BinaryInputArchive in(out.getBuffer(),out.getSize());
	in.readString(str,11);
	printf("--- : %s\n",str.c_str());
	in.readString(str,11);
	printf("--- : %s\n",str.c_str());
	in.readString(str,11);
	printf("--- : %s\n",str.c_str());

	in.readBool(b);
	printf("bool   : %d\n",b);
	in.readBool(b);
	printf("bool   : %d\n",b);

	char c;
	in.readByte(c);
	printf("char   : %c\n",c);
	in.readByte(c);
	printf("char   : %c\n",c);

	double d;
	in.readDouble(d);
	printf("double   : %f\n",d);
	in.readDouble(d);
	printf("double   : %f\n",d);

	float f;
	in.readFloat(f);
	printf("float    : %f\n",f);
	in.readFloat(f);
	printf("float    : %f\n",f);

	int i;
	in.readInt32(i);
	printf("int      :%d\n",i);
	in.readInt32(i);
	printf("int      :%d\n",i);

	__int64 i64;
	in.readInt64(i64);
	printf("int64    %llu\n",i64);
}
#endif