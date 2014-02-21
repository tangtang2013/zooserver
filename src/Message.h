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
	Message() : magicid(MAGIC_ID),
		output(NULL),
		input(NULL)
	{}
	Message(MessageType t) : type(t), 
		magicid(MAGIC_ID),
		output(NULL),
		input(NULL)
	{}
	virtual ~Message(){
		delete output;
		delete input;
	}

	virtual void serialize() = 0;
	virtual void deserialize() = 0;

	static Message* createMessage(MessageType type);
	static Message* createMessage(char* buf, int size);

	MessageType getType(){return type;}

	int getBufferSize(){
		return output ? output->getSize() : 0;
	}

	char* getBuffer(){
		return output ? output->getBuffer() : NULL;
	}

protected:
	BinaryOutputArchive* output;
	BinaryInputArchive* input;

	MessageType type;
	int magicid;
};

class PutRequestMessage : public Message{
public:
	PutRequestMessage();
	PutRequestMessage(char* buf, int size);
	~PutRequestMessage();

	void serialize();
	void deserialize();

	int getKeySize(){return keySize;}
	int getValueSize(){return valueSize;}
	char* getKey(){return key;}
	char* getValue(){return value;}

	void setKey(char* p, int size){key = p;keySize = size;}
	void setValue(char* p, int size){value = p;valueSize = size;}

private:
	int keySize;
	int valueSize;
	char* key;
	char* value;
};

class PutReplyMessage : public Message{
public:
	PutReplyMessage();
	PutReplyMessage(char* buf, int size);
	~PutReplyMessage();

	void serialize();
	void deserialize();

	int getRet(){return ret;}

private:
	int ret;
};

class GetRequestMessage : public Message{
public:
	GetRequestMessage();
	GetRequestMessage(char* buf, int size);
	~GetRequestMessage();

	void serialize();
	void deserialize();

	int getKeySize(){return keySize;}
	char* getKey(){return key;}
private:
	int keySize;
	char* key;
};

class GetReplyMessage : public Message{
public:
	GetReplyMessage();
	GetReplyMessage(char* buf, int size);
	~GetReplyMessage();

	void serialize();
	void deserialize();

	int getRet(){return ret;}
	int getValueSize(){return valueSize;}
	char* getValue(){return value;}
private:
	int ret;
	int valueSize;
	char* value;
};

#endif