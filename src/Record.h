#ifndef _RECORD_H_
#define _RECORD_H_

#include "OutputArchive.h"
#include "InputArchive.h"

class Record
{
public:
	void serialize(OutputArchive* archive, string tag);
	void deserialize(InputArchive* archive, string tag);
};

#endif