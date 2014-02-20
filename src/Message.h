#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "BinaryInputArchive.h"
#include "BinaryOutputArchive.h"

#define MAGIC_ID 0xabcd

enum MessageType{
	PUTREQUESTMSG,
	PUTREPLYMSG,
	GETREQUESTMSG,
	GETREPLYMSG
};

class Message{
public:
	Message() : magicid(MAGIC_ID){}
	Message(MessageType t) : type(t), magicid(MAGIC_ID){}
	virtual ~Message(){}
	Message* createMessage();

	MessageType getType(){return type;}

	virtual int getBufferSize() = 0;
	virtual char* getBuffer() = 0;

protected:
	MessageType type;
	int magicid;
};

class PutRequestMessage : public Message{
public:
	PutRequestMessage();
	~PutRequestMessage();

	void serialize();
	void deserialize();

	int getBufferSize();
	char* getBuffer();

private:
	BinaryOutputArchive* output;
	BinaryInputArchive* input;
	int keySize;
	int valueSize;
	char* key;
	char* value;
};

#endif