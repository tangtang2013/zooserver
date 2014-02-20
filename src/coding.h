#ifndef _TANG_CODING_H_
#define _TANG_CODING_H_

#include "stdint.h"
#include <string>
#include <string.h>

#include "slice.h"
//#include "port.h"

#define LITTLE_ENDIAN

#ifdef LITTLE_ENDIAN
#define IS_LITTLE_ENDIAN true
#else
#define IS_LITTLE_ENDIAN (__BYTE_ORDER == __LITTLE_ENDIAN)
#endif

namespace tang{

	static const bool kLittleEndian = IS_LITTLE_ENDIAN;
	
	extern void PutFixed32(std::string* dst, uint32_t value);
	extern void PutFixed64(std::string* dst, uint64_t value);
	extern void PutVarint32(std::string* dst, uint32_t value);
	extern void PutVarint64(std::string* dst, uint64_t value);
	extern void PutLengthPrefixedSlice(std::string* dst, const Slice& value);

	extern bool GetVarint32(Slice* input, uint32_t* value);
	extern bool GetVarint64(Slice* input, uint64_t* value);
	extern bool GetLengthPrefixedSlice(Slice* input, Slice* result);

	extern const char* GetVarint32Ptr(const char* p,const char* limit, uint32_t* v);
	extern const char* GetVarint64Ptr(const char* p,const char* limit, uint64_t* v);

	extern int VarintLength(uint64_t v);

	extern void EncodeFixed8(char* dst, uint8_t value);
	extern void EncodeFixed32(char* dst, uint32_t value);
	extern void EncodeFixed64(char* dst, uint64_t value);

	extern char* EncodeVarint32(char* dst, uint32_t value);
	extern char* EncodeVarint64(char* dst, uint64_t value);

	inline uint8_t DecodeFixed8(const char* ptr) {
		if (kLittleEndian) {
			// Load the raw bytes
			uint8_t result;
			memcpy(&result, ptr, sizeof(result));  // gcc optimizes this to a plain load
			return result;
		} else {
			return static_cast<uint32_t>(ptr[0]);
		}
	}

	inline uint32_t DecodeFixed32(const char* ptr) {
		if (kLittleEndian) {
			// Load the raw bytes
			uint32_t result;
			memcpy(&result, ptr, sizeof(result));  // gcc optimizes this to a plain load
			return result;
		} else {
			return ((static_cast<uint32_t>(ptr[0]))
				| (static_cast<uint32_t>(ptr[1]) << 8)
				| (static_cast<uint32_t>(ptr[2]) << 16)
				| (static_cast<uint32_t>(ptr[3]) << 24));
		}
	}

	inline uint64_t DecodeFixed64(const char* ptr) {
		if (kLittleEndian) {
			// Load the raw bytes
			uint64_t result;
			memcpy(&result, ptr, sizeof(result));  // gcc optimizes this to a plain load
			return result;
		} else {
			uint64_t lo = DecodeFixed32(ptr);
			uint64_t hi = DecodeFixed32(ptr + 4);
			return (hi << 32) | lo;
		}
	}

	extern const char* GetVarint32PtrFallback(const char* p,
		const char* limit,
		uint32_t* value);
	inline const char* GetVarint32Ptr(const char* p,
		const char* limit,
		uint32_t* value) {
			if (p < limit) {
				uint32_t result = *(reinterpret_cast<const unsigned char*>(p));
				if ((result & 128) == 0) {
					*value = result;
					return p + 1;
				}
			}
			return GetVarint32PtrFallback(p, limit, value);
	}
}

#endif