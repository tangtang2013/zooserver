#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <limits.h>
//#include <sys/time.h>
#include <ctype.h>
#include "dict.h"
#include <WinSock2.h>

static int dict_can_resize = 1;
static unsigned int dict_force_resize_ratio = 5;

static uint32_t dict_hash_function_seed = 5381;

/* MurmurHash2, by Austin Appleby
* Note - This code makes a few assumptions about how your machine behaves -
* 1. We can read a 4-byte value from any address without crashing
* 2. sizeof(int) == 4
*
* And it has a few limitations -
*
* 1. It will not work incrementally.
* 2. It will not produce the same results on little-endian and big-endian
*    machines.
*/
unsigned int dict::dictGenHashFunction(const void *key, int len) {
	/* 'm' and 'r' are mixing constants generated offline.
	They're not really 'magic', they just happen to work well.  */
	uint32_t seed = dict_hash_function_seed;
	const uint32_t m = 0x5bd1e995;
	const int r = 24;

	/* Initialize the hash to a 'random' value */
	uint32_t h = seed ^ len;

	/* Mix 4 bytes at a time into the hash */
	const unsigned char *data = (const unsigned char *)key;

	while(len >= 4) {
		uint32_t k = *(uint32_t*)data;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

	/* Handle the last few bytes of the input array  */
	switch(len) {
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0]; h *= m;
	};

	/* Do a few final mixes of the hash to ensure the last few
	* bytes are well-incorporated. */
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return (unsigned int)h;
}


/* ----------------------------- API implementation ------------------------- */

/* Reset a hash table already initialized with ht_init().
* NOTE: This function should only be called by ht_destroy(). */
void dictht::dictReset()
{
	table = NULL;
	size = 0;
	sizemask = 0;
	used = 0;
}

/* Create a new hash table */
dict::dict()
{
	//dict *d = malloc(sizeof(*d));

	//_dictInit(d,type,privDataPtr);
	dictInit();

}

/* Initialize the hash table */
int dict::dictInit()
{
	ht[0].dictReset();
	ht[1].dictReset();
	//d->type = type;
	//d->privdata = privDataPtr;
	rehashidx = -1;
	iterators = 0;
	return DICT_OK;
}

/* Resize the table to the minimal size that contains all the elements,
* but with the invariant of a USED/BUCKETS ratio near to <= 1 */
int dict::dictResize()
{
	int minimal;

	if (!dict_can_resize || dictIsRehashing()) return DICT_ERR;
	minimal = ht[0].used;
	if (minimal < DICT_HT_INITIAL_SIZE)
		minimal = DICT_HT_INITIAL_SIZE;
	return dictExpand(minimal);
}

unsigned int dict::dictHashKey(const void *key){
	return dictGenHashFunction(key, strlen((const char*)key));
}

/* Expand or create the hash table */
int dict::dictExpand(unsigned long size)
{
	dictht n; /* the new hash table */
	unsigned long realsize = dictNextPower(size);

	/* the size is invalid if it is smaller than the number of
	* elements already inside the hash table */
	if (dictIsRehashing() || ht[0].used > size)
		return DICT_ERR;

	/* Allocate the new hash table and initialize all pointers to NULL */
	n.size = realsize;
	n.sizemask = realsize-1;
	n.table = (dictEntry**)calloc(realsize,sizeof(dictEntry*));
	n.used = 0;

	/* Is this the first initialization? If so it's not really a rehashing
	* we just set the first hash table so that it can accept keys. */
	if (ht[0].table == NULL) {
		ht[0] = n;
		return DICT_OK;
	}

	/* Prepare a second hash table for incremental rehashing */
	ht[1] = n;
	rehashidx = 0;
	return DICT_OK;
}

/* Performs N steps of incremental rehashing. Returns 1 if there are still
* keys to move from the old to the new hash table, otherwise 0 is returned.
* Note that a rehashing step consists in moving a bucket (that may have more
* thank one key as we use chaining) from the old to the new hash table. */
int dict::dictRehash(int n) {
	if (!dictIsRehashing()) return 0;

	while(n--) {
		dictEntry *de, *nextde;

		/* Check if we already rehashed the whole table... */
		if (ht[0].used == 0) {
			free(ht[0].table);
			ht[0] = ht[1];
			ht[1].dictReset();
			rehashidx = -1;
			return 0;
		}

		/* Note that rehashidx can't overflow as we are sure there are more
		* elements because ht[0].used != 0 */
		assert(ht[0].size > (unsigned)rehashidx);
		while(ht[0].table[rehashidx] == NULL) rehashidx++;
		de = ht[0].table[rehashidx];
		/* Move all the keys in this bucket from the old to the new hash HT */
		while(de) {
			unsigned int h;

			nextde = de->next;
			/* Get the index in the new hash table */
			h = dictHashKey(de->key) & ht[1].sizemask;
			de->next = ht[1].table[h];
			ht[1].table[h] = de;
			ht[0].used--;
			ht[1].used++;
			de = nextde;
		}
		ht[0].table[rehashidx] = NULL;
		rehashidx++;
	}
	return 1;
}

long long timeInMilliseconds(void) {
#ifdef _WIN32
	return GetTickCount();
#else
	struct timeval tv;

	gettimeofday(&tv,NULL);
	return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
#endif
}

/* Rehash for an amount of time between ms milliseconds and ms+1 milliseconds */
int dict::dictRehashMilliseconds(int ms) {
	long long start = timeInMilliseconds();
	int rehashes = 0;

	while(dictRehash(100)) {
		rehashes += 100;
		if (timeInMilliseconds()-start > ms) break;
	}
	return rehashes;
}

/* This function performs just a step of rehashing, and only if there are
* no safe iterators bound to our hash table. When we have iterators in the
* middle of a rehashing we can't mess with the two hash tables otherwise
* some element can be missed or duplicated.
*
* This function is called by common lookup or update operations in the
* dictionary so that the hash table automatically migrates from H1 to H2
* while it is actively used. */
void dict::dictRehashStep() {
	if (iterators == 0) dictRehash(1);
}

/* Add an element to the target hash table */
int dict::dictAdd(void *key, void *val)
{
	dictEntry *entry = dictAddRaw(key);

	if (!entry) return DICT_ERR;
	//dictSetVal(d, entry, val);
	entry->v.val = val;
	return DICT_OK;
}

/* Low level add. This function adds the entry but instead of setting
* a value returns the dictEntry structure to the user, that will make
* sure to fill the value field as he wishes.
*
* This function is also directly exposed to user API to be called
* mainly in order to store non-pointers inside the hash value, example:
*
* entry = dictAddRaw(dict,mykey);
* if (entry != NULL) dictSetSignedIntegerVal(entry,1000);
*
* Return values:
*
* If key already exists NULL is returned.
* If key was added, the hash entry is returned to be manipulated by the caller.
*/
dictEntry *dict::dictAddRaw(void *key)
{
	int index;
	dictEntry *entry;
	dictht *dht = NULL;

	if (dictIsRehashing()) dictRehashStep();

	/* Get the index of the new element, or -1 if
	* the element already exists. */
	if ((index = dictKeyIndex(key)) == -1)
		return NULL;

	/* Allocate the memory and store the new entry */
	dht = dictIsRehashing() ? &ht[1] : &ht[0];
	entry = (dictEntry*)malloc(sizeof(*entry));
	entry->next = dht->table[index];
	dht->table[index] = entry;
	dht->used++;

	/* Set the hash entry fields. */
	//dictSetKey(d, entry, key);
	entry->key = key;
	return entry;
}

/* Add an element, discarding the old if the key already exists.
* Return 1 if the key was added from scratch, 0 if there was already an
* element with such key and dictReplace() just performed a value update
* operation. */
int dict::dictReplace(void *key, void *val)
{
	dictEntry *entry, auxentry;

	/* Try to add the element. If the key
	* does not exists dictAdd will suceed. */
	if (dictAdd(key, val) == DICT_OK)
		return 1;
	/* It already exists, get the entry */
	entry = dictFind(key);
	/* Set the new value and free the old one. Note that it is important
	* to do that in this order, as the value may just be exactly the same
	* as the previous one. In this context, think to reference counting,
	* you want to increment (set), and then decrement (free), and not the
	* reverse. */
	auxentry = *entry;
	entry->dictSetVal(val);
	auxentry.dictFreeVal();

	return 0;
}

/* dictReplaceRaw() is simply a version of dictAddRaw() that always
* returns the hash entry of the specified key, even if the key already
* exists and can't be added (in that case the entry of the already
* existing key is returned.)
*
* See dictAddRaw() for more information. */
dictEntry *dict::dictReplaceRaw(void *key) {
	dictEntry *entry = dictFind(key);

	return entry ? entry : dictAddRaw(key);
}

/* Search and remove an element */
int dict::dictGenericDelete(const void *key, int nofree)
{
	unsigned int h, idx;
	dictEntry *he, *prevHe;
	int table;

	if (ht[0].size == 0) return DICT_ERR; /* d->ht[0].table is NULL */
	if (dictIsRehashing()) dictRehashStep();
	h = dictHashKey(key);

	for (table = 0; table <= 1; table++) {
		idx = h & ht[table].sizemask;
		he = ht[table].table[idx];
		prevHe = NULL;
		while(he) {
			if (dictCompareKeys(key, he->key)) {
				/* Unlink the element from the list */
				if (prevHe)
					prevHe->next = he->next;
				else
					ht[table].table[idx] = he->next;
				if (!nofree) {
					he->dictFreeKey();
					he->dictFreeVal();
				}
				free(he);
				ht[table].used--;
				return DICT_OK;
			}
			prevHe = he;
			he = he->next;
		}
		if (!dictIsRehashing()) break;
	}
	return DICT_ERR; /* not found */
}

int dict::dictDelete(const void *key) {
	return dictGenericDelete(key,0);
}

int dict::dictDeleteNoFree(const void *key) {
	return dictGenericDelete(key,1);
}

/* Destroy an entire dictionary */
int dictht::dictClear()
{
	unsigned long i;

	/* Free all the elements */
	for (i = 0; i < size && used > 0; i++) {
		dictEntry *he, *nextHe;

		if ((he = table[i]) == NULL) continue;
		while(he) {
			nextHe = he->next;
			he->dictFreeKey();
			he->dictFreeVal();
			free(he);
			used--;
			he = nextHe;
		}
	}
	/* Free the table and the allocated cache structure */
	free(table);
	/* Re-initialize the table */
	dictReset();
	return DICT_OK; /* never fails */
}

/* Clear & Release the hash table */
void dict::dictRelease(dict *d)
{
	ht[0].dictClear();
	ht[1].dictClear();
}

dictEntry *dict::dictFind(const void *key)
{
	dictEntry *he;
	unsigned int h, idx, table;

	if (ht[0].size == 0) return NULL; /* We don't have a table at all */
	if (dictIsRehashing()) dictRehashStep();
	h = dictHashKey(key);
	for (table = 0; table <= 1; table++) {
		idx = h & ht[table].sizemask;
		he = ht[table].table[idx];
		while(he) {
			if (dictCompareKeys(key, he->key))
				return he;
			he = he->next;
		}
		if (!dictIsRehashing()) return NULL;
	}
	return NULL;
}

void *dict::dictFetchValue(const void *key) {
	dictEntry *he;

	he = dictFind(key);
	return he ? he->dictGetVal() : NULL;
}

dictIterator *dict::dictGetIterator()
{
	dictIterator *iter = new dictIterator();

	iter->d = this;
	iter->table = 0;
	iter->index = -1;
	iter->safe = 0;
	iter->entry = NULL;
	iter->nextEntry = NULL;
	return iter;
}

dictIterator *dict::dictGetSafeIterator() {
	dictIterator *i = dictGetIterator();

	i->safe = 1;
	return i;
}

dictEntry *dictIterator::dictNext()
{
	while (1) {
		if (entry == NULL) {
			dictht *ht = &d->ht[table];
			if (safe && index == -1 && table == 0)
				d->iterators++;
			index++;
			if (index >= (signed) ht->size) {
				if (d->dictIsRehashing() && table == 0) {
					table++;
					index = 0;
					ht = &d->ht[1];
				} else {
					break;
				}
			}
			entry = ht->table[index];
		} else {
			entry = nextEntry;
		}
		if (entry) {
			//* We need to save the 'next' here, the iterator user
			//* may delete the entry we are returning.
			nextEntry = entry->next;
			return entry;
		}
	}
	return NULL;
}

void dictIterator::dictRelease()
{
	if (safe && !(index == -1 && table == 0))
		d->iterators--;
	//free(iter);
}

int random(){
	return 1;
}

/* Return a random entry from the hash table. Useful to
* implement randomized algorithms */
dictEntry *dict::dictGetRandomKey()
{
	dictEntry *he, *orighe;
	unsigned int h;
	int listlen, listele;

	if (dictSize() == 0) return NULL;
	if (dictIsRehashing()) dictRehashStep();
	if (dictIsRehashing()) {
		do {
			h = random() % (ht[0].size+ht[1].size);
			he = (h >= ht[0].size) ? ht[1].table[h - ht[0].size] :
				ht[0].table[h];
		} while(he == NULL);
	} else {
		do {
			h = random() & ht[0].sizemask;
			he = ht[0].table[h];
		} while(he == NULL);
	}

	/* Now we found a non empty bucket, but it is a linked
	* list and we need to get a random element from the list.
	* The only sane way to do so is counting the elements and
	* select a random index. */
	listlen = 0;
	orighe = he;
	while(he) {
		he = he->next;
		listlen++;
	}
	listele = random() % listlen;
	he = orighe;
	while(listele--) he = he->next;
	return he;
}

/* ------------------------- private functions ------------------------------ */

/* Expand the hash table if needed */
int dict::dictExpandIfNeeded()
{
	/* Incremental rehashing already in progress. Return. */
	if (dictIsRehashing()) return DICT_OK;

	/* If the hash table is empty expand it to the initial size. */
	if (ht[0].size == 0) return dictExpand(DICT_HT_INITIAL_SIZE);

	/* If we reached the 1:1 ratio, and we are allowed to resize the hash
	* table (global setting) or we should avoid it but the ratio between
	* elements/buckets is over the "safe" threshold, we resize doubling
	* the number of buckets. */
	if (ht[0].used >= ht[0].size &&
		(dict_can_resize ||
		ht[0].used/ht[0].size > dict_force_resize_ratio))
	{
		return dictExpand(((ht[0].size > ht[0].used) ?
			ht[0].size : ht[0].used)*2);
	}
	return DICT_OK;
}

/* Our hash table capability is a power of two */
unsigned long dict::dictNextPower(unsigned long size)
{
	unsigned long i = DICT_HT_INITIAL_SIZE;

	if (size >= LONG_MAX) return LONG_MAX;
	while(1) {
		if (i >= size)
			return i;
		i *= 2;
	}
}

/* Returns the index of a free slot that can be populated with
* an hash entry for the given 'key'.
* If the key already exists, -1 is returned.
*
* Note that if we are in the process of rehashing the hash table, the
* index is always returned in the context of the second (new) hash table. */
int dict::dictKeyIndex(const void *key)
{
	unsigned int h, idx, table;
	dictEntry *he;

	/* Expand the hash table if needed */
	if (dictExpandIfNeeded() == DICT_ERR)
		return -1;
	/* Compute the key hash value */
	h = dictHashKey(key);
	for (table = 0; table <= 1; table++) {
		idx = h & ht[table].sizemask;
		/* Search if this slot does not already contain the given key */
		he = ht[table].table[idx];
		while(he) {
			if (dictCompareKeys(key, he->key))
				return -1;
			he = he->next;
		}
		if (!dictIsRehashing()) break;
	}
	return idx;
}

void dict::dictEmpty() {
	ht[0].dictClear();
	ht[1].dictClear();
	rehashidx = -1;
	iterators = 0;
}

void dict::dictEnableResize() {
	dict_can_resize = 1;
}

void dict::dictDisableResize() {
	dict_can_resize = 0;
}

void dictEntry::dictSetKey(void* key, int dup){
	this->key = key;
}

void dictEntry::dictSetVal(void* value, int dup){
	this->v.val = value;
}

int dictCompareKeys(const void* key1, const void* key2){
	return strcmp((const char *)key1, (const char *)key2) == 0;
}

#if 0

/* The following is code that we don't use for Redis currently, but that is part
of the library. */

/* ----------------------- Debugging ------------------------*/

#define DICT_STATS_VECTLEN 50
static void _dictPrintStatsHt(dictht *ht) {
	unsigned long i, slots = 0, chainlen, maxchainlen = 0;
	unsigned long totchainlen = 0;
	unsigned long clvector[DICT_STATS_VECTLEN];

	if (ht->used == 0) {
		printf("No stats available for empty dictionaries\n");
		return;
	}

	for (i = 0; i < DICT_STATS_VECTLEN; i++) clvector[i] = 0;
	for (i = 0; i < ht->size; i++) {
		dictEntry *he;

		if (ht->table[i] == NULL) {
			clvector[0]++;
			continue;
		}
		slots++;
		/* For each hash entry on this slot... */
		chainlen = 0;
		he = ht->table[i];
		while(he) {
			chainlen++;
			he = he->next;
		}
		clvector[(chainlen < DICT_STATS_VECTLEN) ? chainlen : (DICT_STATS_VECTLEN-1)]++;
		if (chainlen > maxchainlen) maxchainlen = chainlen;
		totchainlen += chainlen;
	}
	printf("Hash table stats:\n");
	printf(" table size: %ld\n", ht->size);
	printf(" number of elements: %ld\n", ht->used);
	printf(" different slots: %ld\n", slots);
	printf(" max chain length: %ld\n", maxchainlen);
	printf(" avg chain length (counted): %.02f\n", (float)totchainlen/slots);
	printf(" avg chain length (computed): %.02f\n", (float)ht->used/slots);
	printf(" Chain length distribution:\n");
	for (i = 0; i < DICT_STATS_VECTLEN-1; i++) {
		if (clvector[i] == 0) continue;
		printf("   %s%ld: %ld (%.02f%%)\n",(i == DICT_STATS_VECTLEN-1)?">= ":"", i, clvector[i], ((float)clvector[i]/ht->size)*100);
	}
}

void dictPrintStats(dict *d) {
	_dictPrintStatsHt(&d->ht[0]);
	if (dictIsRehashing(d)) {
		printf("-- Rehashing into ht[1]:\n");
		_dictPrintStatsHt(&d->ht[1]);
	}
}

#endif
