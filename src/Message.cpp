#include "Message.h"

Message* Message::createMessage(){
	Message* msg;
	msg = new PutRequestMessage();
	return msg;
}

PutRequestMessage::PutRequestMessage() : 
	Message(PUTREQUESTMSG),
	output(NULL), input(NULL),
	key(NULL), value(NULL),
	keySize(0), valueSize(0)
{
	if(PUTREQUESTMSG){
		output = new BinaryOutputArchive();
	}
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

int PutRequestMessage::getBufferSize(){
	return output ? output->getSize() : 0;
}

char* PutRequestMessage::getBuffer(){
	return output ? output->getBuffer() : NULL;
}

void PutRequestMessage::serialize(){
	output->writeByte(type);
	output->writeInt32(magicid);
	output->writeInt32(keySize);
	output->writeInt32(valueSize);
	output->writeString(key,keySize);
	output->writeString(value,valueSize);
}

void PutRequestMessage::deserialize(){
	//input->readByte(type);
	input->readInt32(magicid);
	input->readInt32(keySize);
	input->readInt32(valueSize);
	key = (char*)malloc(keySize);
	value = (char*)malloc(valueSize);
	input->readString(key,keySize);
	input->readString(value,valueSize);
}