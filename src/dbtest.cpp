#include <stdio.h>
#include "Dict.h"

//#define _CRT_SECURE_NO_DEPRECATE

int main(int argc, char** argv){
	Dict* d = new Dict();
	d->dictAdd(_strdup("hello"),_strdup("world"));
	d->dictAdd(strdup("hello1"),strdup("world1"));
	d->dictAdd(strdup("hello2"),strdup("world2"));
	d->dictAdd(strdup("hello3"),strdup("world3"));
	d->dictAdd(strdup("hello4"),strdup("world4"));
	d->dictAdd(strdup("hello5"),strdup("world5"));
	d->dictAdd(strdup("hello6"),strdup("world6"));
	d->dictAdd(strdup("hello7"),strdup("world7"));
	d->dictAdd(strdup("hello8"),strdup("world8"));
	d->dictAdd(strdup("hello9"),strdup("world9"));
	d->dictAdd(strdup("hello10"),strdup("world10"));
	d->dictAdd(strdup("hello11"),strdup("world11"));
	d->dictAdd(strdup("hello12"),strdup("world12"));
	printf("find : %s\n",d->dictFind("hello")->dictGetVal());

	DictIterator* iterator = d->dictGetIterator();
	DictEntry* entry = NULL;
	while(entry = iterator->dictNext()){
		printf("iterator %s : %s\n",entry->dictGetKey(), entry->dictGetVal());
	}
	d->dictEmpty();
	printf("find : %s\n",d->dictFind("hello"));
}