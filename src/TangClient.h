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
	 * ���³�ʼ��client
	 * @param [in] eventLoop ae�¼��������
	 * @param [in] fd server��socket�ļ����
	 * @param [in] tangServer server����
	 */
	void reInitUseParam(struct aeEventLoop *eventLoop, int fd, TangServer* tangServer);
	void read();
	void write();
	int init();

	/*
	 * ������յ���buf
	 */
	void cmdHandler();

	/*
	 * ��ȡclient socket���
	 * @return socket���
	 */
	int getSocketFd();

	/*
	 * �ر�client����
	 */
	void close();

private:
	/*
	 * read�ص�����������read����
	 * @param [in] eventLoop ae�¼��������
	 * @param [in] fd server��socket�ļ����
	 * @param [in] clientData ��̬��������ֱ�ӷ���class��Ա�����Խ�thisָ�봫��ص�����
	 * @param [in] mask ע��Զ�����дsocket�¼�
	 */
	static void readFileProc(struct aeEventLoop *eventLoop, int sfd, void *clientData, int mask);

private:
	int clientfd;						//client socket���
	int reqtype;						//
	int flags;							//
	int bufpos;							//
	char buf[REDIS_REPLY_CHUNK_BYTES];	//socket�յ�������
	char err[128];						//err��Ϣ

	ClientState state;					//client״̬
	aeEventLoop* el;					//����socket�¼�
	TangServer* server;
};

#endif