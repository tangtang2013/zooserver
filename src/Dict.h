/* Hash Tables Implementation.
*
* This file implements in-memory hash tables with insert/del/replace/find/
* get-random-element operations. Hash tables will auto-resize if needed
* tables of power of two in size are used, collisions are handled by
* chaining. See the source code for more information... :)
*
* Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*   * Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*   * Neither the name of Redis nor the names of its contributors may be used
*     to endorse or promote products derived from this software without
*     specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#ifndef __DICT_H
#define __DICT_H

#define DICT_OK 0
#define DICT_ERR 1

/* Unused arguments generate annoying warnings... */
#define DICT_NOTUSED(V) ((void) V)

typedef struct dictEntry {
	/*
	 * ����key
	 * @param [in] keyָ��
	 * @param [in] dup��0��ָ�븳ֵ��1��dup���ڴ�
	 */
	void dictSetKey(void* key, int dup=0);

	/*
	 * ����union�е�int64ֵ
	 * @param [in] value
	 */
	void dictSetSignedIntegerVal(int64_t val){v.s64 = val;}

	/*
	 * ����union�е�uint64ֵ
	 * @param [in] value
	 */
	void dictSetUnsignedIntegerVal(uint64_t val){v.u64 = val;}

	/*
	 * ����union�е�valָ���ַ
	 * @param [in] value
	 * @param [in] dup��0��ָ�븳ֵ��1��dup���ڴ�
	 */
	void dictSetVal(void* value, int dup=0);

	/*
	 * ��ȡvalue
	 * @return ����union�е�valָ��
	 */
	void* dictGetVal(){return v.val;}

	/*
	 * ��ȡkey
	 * @return ����keyָ��
	 */
	void* dictGetKey(){return key;}

	/*
	 * free�ͷ�key�ڴ�
	 */
	void dictFreeKey(){free(key);}

	/*
	 * free�ͷ�value�ڴ�
	 */
	void dictFreeVal(){free(v.val);}
	void *key;
	union {
		void *val;
		uint64_t u64;
		int64_t s64;
	} v;
	struct dictEntry *next;
} dictEntry;

typedef struct dictType {
	unsigned int (*hashFunction)(const void *key);
	void *(*keyDup)(void *privdata, const void *key);
	void *(*valDup)(void *privdata, const void *obj);
	int (*keyCompare)(void *privdata, const void *key1, const void *key2);
	void (*keyDestructor)(void *privdata, void *key);
	void (*valDestructor)(void *privdata, void *obj);
} dictType;

/* This is our hash table structure. Every dictionary has two of this as we
* implement incremental rehashing, for the old to the new table. */
typedef struct dictht {
	void dictReset();
	int dictClear();
	dictEntry **table;
	unsigned long size;
	unsigned long sizemask;
	unsigned long used;
} dictht;

class dictIterator;

class dict {
	friend class dictIterator;
public:
	dict();
	~dict();
	/* API */
	int dictExpand(unsigned long size);

	int dictAdd(void *key, void *val);
	dictEntry *dictAddRaw(void *key);
	int dictReplace(void *key, void *val);
	dictEntry *dictReplaceRaw(void *key);
	int dictDelete(const void *key);
	int dictDeleteNoFree(const void *key);
	void dictRelease(dict *d);
	dictEntry * dictFind(const void *key);
	void *dictFetchValue(const void *key);

	int dictResize();
	dictIterator *dictGetIterator();
	dictIterator *dictGetSafeIterator();
	dictEntry *dictNext(dictIterator *iter);
	void dictReleaseIterator(dictIterator *iter);
	dictEntry *dictGetRandomKey();
	
	void dictPrintStats(dict *d);
	unsigned int dictGenHashFunction(const void *key, int len);
	unsigned int dictGenCaseHashFunction(const unsigned char *buf, int len);
	void dictEmpty();
	void dictEnableResize(void);
	void dictDisableResize(void);
	int dictRehash(int n);
	int dictRehashMilliseconds(int ms);
	void dictSetHashFunctionSeed(unsigned int initval);
	unsigned int dictGetHashFunctionSeed(void);

	//---
	unsigned int dictHashKey(const void *key);

	unsigned int dictSlots(){return (ht[0].size+ht[1].size);}
	unsigned int dictSize(){return (ht[0].used+ht[1].used);}
	int dictIsRehashing(){return (rehashidx != -1);}

private:
	int dictExpandIfNeeded(dict *ht);
	unsigned long dictNextPower(unsigned long size);
	int dictKeyIndex(const void *key);
	int dictInit();
	void dictRehashStep();
	int dictGenericDelete(const void *key, int nofree);
	int dictExpandIfNeeded();

private:
	dictType *type;
	void *privdata;
	dictht ht[2];
	int rehashidx; /* rehashing not in progress if rehashidx == -1 */
	int iterators; /* number of iterators currently running */
};

/* If safe is set to 1 this is a safe iterator, that means, you can call
* dictAdd, dictFind, and other functions against the dictionary even while
* iterating. Otherwise it is a non safe iterator, and only dictNext()
* should be called while iterating. */
class dictIterator {
	friend class dict;
public:
	/*
	 * ��ȡ��һ��entry
	 * @return һ��entry�����Iterator������������NULL
	 */
	dictEntry* dictNext();

	/*
	 * �ͷ�Iterator
	 */
	void dictRelease();
private:
	dict *d;	//��Ҫ������dict
	int table;	//table ID
	int index;	//��ϣ���ϵ�index
	int safe;	//��ȫ��׼
	dictEntry *entry;
	dictEntry *nextEntry;
};

/* This is the initial size of every hash table */
#define DICT_HT_INITIAL_SIZE     4

/*
 * �Ա��ַ���
 * @param [in] key1
 * @param [in] key2
 * @return ��ͬ����0����ͬ����1
 */
int dictCompareKeys(const void* key1, const void* key2);

#endif /* _DICT_H_ */