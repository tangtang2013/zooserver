#include "Message.h"

Message* Message::createMessage(MessageType type){
	Message* msg= NULL;
	switch(type){
	case PUTREQUESTMSG:
		msg = new PutRequestMessage();
		break;
	case PUTREPLYMSG:
		msg = new PutReplyMessage();
		break;
	case GETREQUESTMSG:
		msg = new GetRequestMessage();
		break;
	case GETREPLYMSG:
		msg = new GetReplyMessage();
		break;
	default:
		printf("Message Type Error : %d\n",type);
		break;
	}

	return msg;
}

Message* Message::createMessage(char* buf, int size){
	Message* msg= NULL;
	int32_t magicid = 0;
	MessageType type;
	
	//检查magicid是否匹配
	memcpy(&magicid, buf, sizeof(int32_t));
	if(magicid != MAGIC_ID){
		return msg;
	}
	//获取Message Type
	memcpy(&type, buf + sizeof(int32_t), sizeof(MessageType));
	switch(type){
	case PUTREQUESTMSG:
		msg = new PutRequestMessage(buf, size);
		break;
	case PUTREPLYMSG:
		msg = new PutReplyMessage(buf, size);
		break;
	case GETREQUESTMSG:
		msg = new GetRequestMessage(buf, size);
		break;
	case GETREPLYMSG:
		msg = new GetReplyMessage(buf, size);
		break;
	default:
		printf("Message Type Error : %d\n",type);
		break;
	}

	return msg;
}

PutRequestMessage::PutRequestMessage() : 
Message(PUTREQUESTMSG),
	key(NULL), value(NULL),
	keySize(0), valueSize(0)
{
		output = new BinaryOutputArchive();
}

PutRequestMessage::PutRequestMessage(char* buf, int size) :
Message(PUTREQUESTMSG),
	key(NULL), value(NULL),
	keySize(0), valueSize(0)
{
		input = new BinaryInputArchive((char*)memcpy(malloc(size),buf,size),size);
}

PutRequestMessage::~PutRequestMessage(){
	if(output){
		delete output;
	}
	if(input){
		delete input;
	}
	if(key){
		free(key);
	}
	if(value){
		free(value);
	}
}

void PutRequestMessage::serialize(){
	output->writeInt32(type);
	output->writeInt32(magicid);
	output->writeInt32(keySize);
	output->writeInt32(valueSize);
	output->writeString(key,keySize);
	output->writeString(value,valueSize);
}

void PutRequestMessage::deserialize(){
	input->readInt32((int32_t&)type);
	input->readInt32(magicid);
	input->readInt32(keySize);
	input->readInt32(valueSize);
	key = (char*)malloc(keySize);
	value = (char*)malloc(valueSize);
	input->readString(key,keySize);
	input->readString(value,valueSize);
}

PutReplyMessage::PutReplyMessage():
	Message(PUTREPLYMSG),
	ret(0){
		output = new BinaryOutputArchive();
}

PutReplyMessage::PutReplyMessage(char* buf, int size):
	Message(PUTREPLYMSG),
	ret(0){
		input = new BinaryInputArchive((char*)memcpy(malloc(size),buf,size),size);
}

PutReplyMessage::~PutReplyMessage(){
	if(output){
		delete output;
	}
	if(input){
		delete input;
	}
}

void PutReplyMessage::serialize(){
	output->writeInt32(type);
	output->writeInt32(magicid);
	output->writeInt32(ret);
}

void PutReplyMessage::deserialize(){
	input->readInt32((int32_t&)type);
	input->readInt32(magicid);
	input->readInt32(ret);
}

//Get Request Message
GetRequestMessage::GetRequestMessage() : 
Message(PUTREQUESTMSG),
	key(NULL), 
	keySize(0)
{
		output = new BinaryOutputArchive();
}

GetRequestMessage::GetRequestMessage(char* buf, int size) :
Message(PUTREQUESTMSG),
	key(NULL), 
	keySize(0)
{
		input = new BinaryInputArchive((char*)memcpy(malloc(size),buf,size),size);
}

GetRequestMessage::~GetRequestMessage(){
	if(output){
		delete output;
	}
	if(input){
		delete input;
	}
	if(key){
		free(key);
	}
}

void GetRequestMessage::serialize(){
	output->writeInt32(type);
	output->writeInt32(magicid);
	output->writeInt32(keySize);
	output->writeString(key,keySize);
}

void GetRequestMessage::deserialize(){
	input->readInt32((int32_t&)type);
	input->readInt32(magicid);
	input->readInt32(keySize);
	key = (char*)malloc(keySize);
	input->readString(key,keySize);
}

//Get Reply Message
GetReplyMessage::GetReplyMessage() : 
Message(PUTREQUESTMSG),
	value(NULL), 
	valueSize(0)
{
		output = new BinaryOutputArchive();
}

GetReplyMessage::GetReplyMessage(char* buf, int size) :
Message(PUTREQUESTMSG),
	value(NULL), 
	valueSize(0)
{
		input = new BinaryInputArchive((char*)memcpy(malloc(size),buf,size),size);
}

GetReplyMessage::~GetReplyMessage(){
	if(output){
		delete output;
	}
	if(input){
		delete input;
	}
	if(value){
		free(value);
	}
}

void GetReplyMessage::serialize(){
	output->writeInt32(type);
	output->writeInt32(magicid);
	output->writeInt32(ret);
	output->writeInt32(valueSize);
	output->writeString(value,valueSize);
}

void GetReplyMessage::deserialize(){
	input->readInt32((int32_t&)type);
	input->readInt32(magicid);
	input->readInt32(ret);
	input->readInt32(valueSize);
	value = (char*)malloc(valueSize);
	input->readString(value,valueSize);
}