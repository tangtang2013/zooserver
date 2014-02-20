#ifndef _TANGCLIENT_H_
#define _TANGCLIENT_H_

#include "../net/ae.h"

#define REDIS_REPLY_CHUNK_BYTES (5*1500) /* 5 TCP packets with default MTU */

enum ClientState{
	NOINIT,
	NORMAL,
	CLOSE
};

class TangServer;

/*********************************
 * @class
 * @brief
**********************************/
class TangClient{
public:
	TangClient(struct aeEventLoop *eventLoop, int fd, TangServer* tangServer);
	~TangClient();

	/*
	 * 重新初始化client
	 * @param [in] eventLoop ae事件处理对象
	 * @param [in] fd server的socket文件句柄
	 * @param [in] tangServer server对象
	 */
	void reInitUseParam(struct aeEventLoop *eventLoop, int fd, TangServer* tangServer);
	void read();
	void write();
	int init();

	/*
	 * 处理接收到的buf
	 */
	void cmdHandler();

	/*
	 * 获取client socket句柄
	 * @return socket句柄
	 */
	int getSocketFd();

	/*
	 * 关闭client连接
	 */
	void close();

private:
	/*
	 * read回调函数，处理read请求
	 * @param [in] eventLoop ae事件处理对象
	 * @param [in] fd server的socket文件句柄
	 * @param [in] clientData 静态函数不能直接访问class成员，所以将this指针传入回调函数
	 * @param [in] mask 注册对读或者写socket事件
	 */
	static void readFileProc(struct aeEventLoop *eventLoop, int sfd, void *clientData, int mask);

private:
	int clientfd;						//client socket句柄
	int reqtype;						//
	int flags;							//
	int bufpos;							//
	char buf[REDIS_REPLY_CHUNK_BYTES];	//socket收到的数据
	char err[128];						//err信息

	ClientState state;					//client状态
	aeEventLoop* el;					//处理socket事件
	TangServer* server;
};

#endif