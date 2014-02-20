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
	 * ����һ��client���ӣ��ȴ�closeclientmapȡ���е�client��Ҫ��û�о����´���client
	 * @param [in] eventLoop ae�¼��������
	 * @param [in] fd client��socket�ļ����
	 * @rturn ����һ��client���Ӷ���
	 */
	TangClient* createClient(struct aeEventLoop *eventLoop, int fd);

	/*
	 * �ر�һ��client����
	 * @param [in] pClient client���Ӷ���
	 */
	void closeClient(TangClient* pClient);

	/*
	 * ������close״̬��client���ӷ���closeclientmap���Ա��������
	 * @param [in] pClient client����
	 */
	void putClientInCloseMap(TangClient* pClient);

private:
	/*
	 * accept�ص�����������accept����
	 * @param [in] eventLoop ae�¼��������
	 * @param [in] fd server��socket�ļ����
	 * @param [in] clientData ��̬��������ֱ�ӷ���class��Ա�����Խ�thisָ�봫��ص�����
	 * @param [in] mask ע��Զ�����дsocket�¼�
	 */
	static void acceptProc(struct aeEventLoop *eventLoop, int sfd, void *clientData, int mask);

private:
	int serverfd;							//server socket���
	int port;								//�˿�
	std::string addr;						//ip��ַ
	char err[128];							//err��Ϣ
	aeEventLoop* el;						//����socket�¼�
	std::map<int,TangClient*> clientmap;	//����client����
	std::map<int,TangClient*> closeclientmap;	//����رյ�client����
};

#endif