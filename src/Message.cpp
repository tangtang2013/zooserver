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
	
}

PutRequestMessage::~PutRequestMessage(){
	
}

int PutRequestMessage::getBufferSize(){
	return 0;
}

char* PutRequestMessage::getBuffer(){
	return NULL;
}