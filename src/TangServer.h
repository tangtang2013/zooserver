#ifndef _TANGSERVER_H_
#define _TANGSERVER_H_

#include <map>
#include <string>
#include "../net/ae.h"
//#include "TangClient.h"
class TangClient;

/*********************************
 * @class
 * @brief
**********************************/
class TangServer{
public:
	TangServer(int port);
	~TangServer();

	int init();
	void start();
	void stop();

	/*
	 * 创建一个client连接，先从closeclientmap取空闲的client，要是没有就重新创建client
	 * @param [in] eventLoop ae事件处理对象
	 * @param [in] fd client的socket文件句柄
	 * @rturn 返回一个client连接对象
	 */
	TangClient* createClient(struct aeEventLoop *eventLoop, int fd);

	/*
	 * 关闭一个client连接
	 * @param [in] pClient client连接对象
	 */
	void closeClient(TangClient* pClient);

	/*
	 * 将处于close状态的client连接放入closeclientmap，以便回收利用
	 * @param [in] pClient client连接
	 */
	void putClientInCloseMap(TangClient* pClient);

private:
	/*
	 * accept回调函数，处理accept请求
	 * @param [in] eventLoop ae事件处理对象
	 * @param [in] fd server的socket文件句柄
	 * @param [in] clientData 静态函数不能直接访问class成员，所以将this指针传入回调函数
	 * @param [in] mask 注册对读或者写socket事件
	 */
	static void acceptProc(struct aeEventLoop *eventLoop, int sfd, void *clientData, int mask);

private:
	int serverfd;							//server socket句柄
	int port;								//端口
	std::string addr;						//ip地址
	char err[128];							//err信息
	aeEventLoop* el;						//处理socket事件
	std::map<int,TangClient*> clientmap;	//管理client连接
	std::map<int,TangClient*> closeclientmap;	//管理关闭的client连接
};

#endif