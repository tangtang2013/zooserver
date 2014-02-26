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
	 * 设置key
	 * @param [in] key指针
	 * @param [in] dup，0是指针赋值，1是dup新内存
	 */
	void dictSetKey(void* key, int dup=0);

	/*
	 * 设置union中的int64值
	 * @param [in] value
	 */
	void dictSetSignedIntegerVal(int64_t val){v.s64 = val;}

	/*
	 * 设置union中的uint64值
	 * @param [in] value
	 */
	void dictSetUnsignedIntegerVal(uint64_t val){v.u64 = val;}

	/*
	 * 设置union中的val指针地址
	 * @param [in] value
	 * @param [in] dup，0是指针赋值，1是dup新内存
	 */
	void dictSetVal(void* value, int dup=0);

	/*
	 * 获取value
	 * @return 返回union中的val指针
	 */
	void* dictGetVal(){return v.val;}

	/*
	 * 获取key
	 * @return 返回key指针
	 */
	void* dictGetKey(){return key;}

	/*
	 * free释放key内存
	 */
	void dictFreeKey(){free(key);}

	/*
	 * free释放value内存
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
	/*
	 * 扩张或者创建hashtable
	 * @param [in] size
	 * @return 返回执行结果，DICT_ERR表示失败，DICT_OK表示成功
	 */
	int dictExpand(unsigned long size);

	/*
	 * 往dict中add数据（key-value）
	 * @param [in] key
	 * @param [in] value
	 * @return 返回执行结果，DICT_ERR表示失败，DICT_OK表示成功
	 */
	int dictAdd(void *key, void *val);
	
	/*
	 * 往dict中add数据（key）
	 * @param [in] key
	 * @return 返回Raw，NULL表示key存在,非NULL表示不存在
	 */
	dictEntry *dictAddRaw(void *key);

	/*
	 * 往dict中替换key对应的value数据
	 * @param [in] key
	 * @param [in] value
	 * @return 返回执行结果，永远返回DICT_OK，DICT_OK表示成功
	 */
	int dictReplace(void *key, void *val);

	/*
	 * 往dict中替换key
	 * @param [in] key
	 * @return 返回Raw,永远不为NULL
	 */
	dictEntry *dictReplaceRaw(void *key);

	/*
	 * 往dict中删除key对应的entry,并且释放entry内存
	 * @param [in] key
	 * @return 返回执行结果，DICT_ERR表示失败，DICT_OK表示成功
	 */
	int dictDelete(const void *key);

	/*
	 * 往dict中删除key对应的entry，不释放entry内存
	 * @param [in] key
	 * @return 返回执行结果，DICT_ERR表示失败，DICT_OK表示成功
	 */
	int dictDeleteNoFree(const void *key);

	/*
	 * release dict
	 */
	void dictRelease();

	/*
	 * 查找key对应的Entry
	 * @param [in] key
	 * @return 成功返回查找到的Entry，找不到为NULL
	 */
	dictEntry * dictFind(const void *key);

	/*
	 * 获取key对应的value
	 * @param [in] key
	 * @return 成功返回查找到的value，找不到为NULL
	 */
	void *dictFetchValue(const void *key);

	/*
	 * 重新调整hashtable的大小
	 * @param [in] key
	 * @return 返回执行结果，DICT_ERR表示失败，DICT_OK表示成功
	 */
	int dictResize();

	/*
	 * 获取Iterator
	 * @return 返回Iterator迭代器指针
	 */
	dictIterator *dictGetIterator();

	/*
	 * 获取安全的Iterator
	 * @return 返回Iterator迭代器指针
	 */
	dictIterator *dictGetSafeIterator();
	
	/*
	 * 获取dict中随机一个Entry
	 * @return 返回Entry指针
	 */
	dictEntry *dictGetRandomKey();
	
	void dictPrintStats(dict *d);
	unsigned int dictGenHashFunction(const void *key, int len);
	unsigned int dictGenCaseHashFunction(const unsigned char *buf, int len);

	/*
	 * 清空dict中的数据
	 */
	void dictEmpty();

	/*
	 * 打开Resize功能
	 */
	void dictEnableResize(void);

	/*
	 * 关闭Resize功能
	 */
	void dictDisableResize(void);

	/*
	 * 执行rehash，扩张之后，每一次操作rehash移动hashtable的一个bucket
	 * @param [in] 执行的步骤数
	 */
	int dictRehash(int n);

	/*
	 * 执行rehash，超过ms时间就结束
	 * @param [in] ms 执行的时间
	 * @return 返回执行的步数
	 */
	int dictRehashMilliseconds(int ms);

	/*
	 * 获取key的hash值
	 * @param [in] key
	 * @return 返回key的hash值
	 */
	unsigned int dictHashKey(const void *key);

	/*
	 * 获取slots数量
	 * @return 返回slot数量
	 */
	unsigned int dictSlots(){return (ht[0].size+ht[1].size);}

	/*
	 * 获取entry数量
	 * @return 返回entry数量
	 */
	unsigned int dictSize(){return (ht[0].used+ht[1].used);}

	/*
	 * 判断是否在进行rehash
	 * @return 返回1或0,0表示没在rehash，1表示正在rehash
	 */
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
	dictht ht[2];	//hashtable，方便expand
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
	 * 获取下一个entry
	 * @return 一个entry，如果Iterator遍历结束返回NULL
	 */
	dictEntry* dictNext();

	/*
	 * 释放Iterator
	 */
	void dictRelease();
private:
	dict *d;	//需要遍历的dict
	int table;	//table ID
	int index;	//哈希表上的index
	int safe;	//安全标准
	dictEntry *entry;
	dictEntry *nextEntry;
};

/* This is the initial size of every hash table */
#define DICT_HT_INITIAL_SIZE     4

/*
 * 对比字符串
 * @param [in] key1
 * @param [in] key2
 * @return 相同返回0，不同返回1
 */
int dictCompareKeys(const void* key1, const void* key2);

#endif /* _DICT_H_ */
