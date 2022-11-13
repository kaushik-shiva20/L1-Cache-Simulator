#include "cache.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <iomanip>
#include <cmath>

using namespace std;
cache::cache(unsigned size, 
      unsigned associativity,
      unsigned line_size,
      write_policy_t wr_hit_policy,
      write_policy_t wr_miss_policy,
      unsigned hit_time,
      unsigned miss_penalty,
      unsigned address_width
){
	/* edit here */
    mCacheSize = size;
    mCacheAssociativity = associativity;
    mCacheLineSize = line_size;
    mCacheWriteHitPolicy = wr_hit_policy;
    mCacheWriteMissPolicy = wr_miss_policy;
    mCacheHitTime = hit_time;
    mCacheMissPenalty = miss_penalty;
    mAddressWidth = address_width;

    // Initialize the cache
    //Number of sets = cache size / (associativity * line size)
    mCacheSets = mCacheSize / (mCacheLineSize * mCacheAssociativity);

    mCache = (cache_line_t **)calloc(mCacheSets,sizeof(cache_line_t *));
    for (unsigned i = 0; i < mCacheSets; i++) {
        mCache[i] = (cache_line_t *)calloc(mCacheAssociativity,sizeof(cache_line_t) );
    }

    mCacheLineOffsetBits = (unsigned) (log(mCacheLineSize)/log(2));
    mCacheLineOffsetMask = ((long long)1 << mCacheLineOffsetBits) - 1;
    mCacheSetIndexBits = (unsigned) (log(mCacheSets)/log(2));
    mCacheSetIndexMask = (((long long)1 << mCacheSetIndexBits) - 1) << mCacheLineOffsetBits;
    mCacheTagBits = mAddressWidth - mCacheSetIndexBits - mCacheLineOffsetBits;
    mCacheTagMask = (((long long)1 << mCacheTagBits) - 1) << (mCacheSetIndexBits + mCacheLineOffsetBits);

    mMemoryAccesses = 0;
    mRead = 0;
    mReadMisses = 0;
    mWrite = 0;
    mWriteMisses = 0;
    mEvictions = 0;
    mMemoryWrites = 0;
    mAvgMemAccessTime = 0;
}

void cache::print_configuration(){
	/* edit here */

    string wr_hit_policy_str[2] = {"write-back", "write-through"};
        string wr_miss_policy_str[2] = {"write-allocate", "no-write-allocate"};
    cout << "CACHE CONFIGURATION" << endl;
    cout << "size = " << (unsigned)mCacheSize/1024 << " KB" << endl;
    cout << "associativity = " << mCacheAssociativity << "-way" << endl;
    cout << "cache line size = " << mCacheLineSize << " B" << endl;
    cout << "write hit policy = " << wr_hit_policy_str[mCacheWriteHitPolicy] << endl;
    cout << "write miss policy = " << wr_miss_policy_str[mCacheWriteMissPolicy-WRITE_ALLOCATE] << endl;
    cout << "cache hit time = " << mCacheHitTime << " CLK" << endl;
    cout << "cache miss penalty = " << mCacheMissPenalty << " CLK" << endl;
    cout << "memory address width = " << mAddressWidth << " bits" << endl;
}

cache::~cache(){
	/* edit here */
    for (unsigned i = 0; i < mCacheSets; i++) {
        free(mCache[i]);
    }
    free(mCache);

    mMemoryAccesses = 0;
    mRead = 0;
    mReadMisses = 0;
    mWrite = 0;
    mWriteMisses = 0;
    mEvictions = 0;
    mMemoryWrites = 0;
    mAvgMemAccessTime = 0;
}

void cache::load_trace(const char *filename){
   stream.open(filename);
   //cout << "stream status" << stream.failbit << endl;
}

void cache::run(unsigned num_entries){

   unsigned first_access = number_memory_accesses;
   string line;
   unsigned line_nr=0;

   while (getline(stream,line)){

	line_nr++;
        char *str = const_cast<char*>(line.c_str());
	
        // tokenize the instruction
    char *op = strtok (str," ");
	char *addr = strtok (NULL, " ");
	address_t address = strtoul(addr, NULL, 16);

/* added */
	//cout << "ADDRESS:: @=0x" << hex << address << endl;

	/* 
		edit here:
		insert the code to process read and write operations
	   	using the read() and write() functions below

	*/
    if(*op == 'r')
    {
        read(address);
    }
    else if(*op == 'w')
    {
        write(address);
    }
    else
    {
        //TODO: cout << "ERROR: Invalid operation" << endl;
    }


	number_memory_accesses++;
	if (num_entries!=0 && (number_memory_accesses-first_access)==num_entries)
		break;
   }
/*
   for(unsigned i=0; i<mCacheSets; i++) {
        for(unsigned j=0; j<mCacheAssociativity; j++) {
            if(mCache[i][j].valid) {
                if(mCache[i][j].dirty) {
                    mMemoryWrites++;
                }
            }
        }
   }
*/
}

void cache::print_statistics(){
	cout << "STATISTICS" << endl;
	/* edit here */

    long read_time = mRead * mCacheHitTime + mReadMisses * mCacheMissPenalty;
    long write_time = mWrite * mCacheHitTime + mWriteMisses * mCacheMissPenalty;
    mAvgMemAccessTime = ((float)(read_time + write_time)) / ((float)(mRead + mWrite));
    //print statistics
    cout << "memory accesses = " << (mRead + mWrite) << endl;
    cout << "read = " << mRead << endl;
    cout << "read misses = " << mReadMisses << endl;
    cout << "write = " << mWrite << endl;
    cout << "write misses = " << mWriteMisses << endl;
    cout << "evictions = " << mEvictions << endl;
    cout << "memory writes = " << mMemoryWrites << endl;
    cout << "average memory access time = " << mAvgMemAccessTime << endl;
}

access_type_t cache::read(address_t address){
	/* edit here */
    long long offset = address & mCacheLineOffsetMask;
    long long set_index = (address & mCacheSetIndexMask) >> mCacheLineOffsetBits;
    long long tag = (address & mCacheTagMask) >> (mCacheSetIndexBits + mCacheLineOffsetBits);
    mRead++;
    for (unsigned i = 0; i < mCacheAssociativity; i++) {
        if (mCache[set_index][i].valid && mCache[set_index][i].tag == tag) {
            //hit
            //update LRU queue
            LRU_update_access(set_index, i);
            return HIT;
        }
    }

    //read from memory
    unsigned tempFreeBlock = 0;
    tempFreeBlock = evict(set_index);
    mCache[set_index][tempFreeBlock].valid = true;
    mCache[set_index][tempFreeBlock].tag = tag;
    mCache[set_index][tempFreeBlock].dirty = false;
    LRU_update_access(set_index, tempFreeBlock);
    mReadMisses++;
    return MISS;

}

access_type_t cache::write(address_t address){
	/* edit here */
    long long offset = address & mCacheLineOffsetMask;
    long long set_index = (address & mCacheSetIndexMask) >> mCacheLineOffsetBits;
    long long tag = (address & mCacheTagMask) >> (mCacheSetIndexBits + mCacheLineOffsetBits);
    mWrite++;
    //write hit handling
    for (unsigned i = 0; i < mCacheAssociativity; i++) {
        if (mCache[set_index][i].valid && mCache[set_index][i].tag == tag) {
            //hit
            LRU_update_access(set_index, i);
            if(mCacheWriteHitPolicy == WRITE_BACK) {
                mCache[set_index][i].dirty = true;
            }else if(mCacheWriteHitPolicy == WRITE_THROUGH) {
                //write to memory and cache
                mCache[set_index][i].dirty = false;
                mMemoryWrites++;

            }
            return HIT;
        }
    }

    //write miss handling
    if(mCacheWriteMissPolicy == WRITE_ALLOCATE) {
        //read from memory
        unsigned tempFreeBlock = 0;
        tempFreeBlock = evict(set_index);
        mCache[set_index][tempFreeBlock].valid = true;
        mCache[set_index][tempFreeBlock].tag = tag;
        mCache[set_index][tempFreeBlock].dirty = true;
        LRU_update_access(set_index, tempFreeBlock);
    } else if(mCacheWriteMissPolicy == NO_WRITE_ALLOCATE)
    {
        //write to memory
        mMemoryWrites++;

    }
    mWriteMisses++;
    return MISS;

}

void cache::print_tag_array(){
	cout << "TAG ARRAY" << endl;
	/* edit here */

    for(unsigned j = 0; j < mCacheAssociativity; j++) {
        cout << "BLOCKS " << j <<endl;
        if((mCacheWriteHitPolicy == WRITE_BACK) || (mCacheWriteMissPolicy == WRITE_ALLOCATE)) {
            cout << setfill(' ') << setw(7) << "index" << setw(6) << "dirty" << setw(4 + (mCacheTagBits/4)) << "tag" << endl;
        } else if((mCacheWriteHitPolicy == WRITE_THROUGH) || (mCacheWriteMissPolicy == NO_WRITE_ALLOCATE)) {
            cout << setfill(' ') << setw(7) << "index" << setw(4 + (mCacheTagBits/4)) << "tag" << endl;
        }

        for(unsigned i = 0; i < mCacheSets; i++){
            if(mCache[i][j].valid) {
                if((mCacheWriteHitPolicy == WRITE_BACK) || (mCacheWriteMissPolicy == WRITE_ALLOCATE)) {
                    cout << setfill(' ') << setw(7) << i << setw(6) << mCache[i][j].dirty << setw(4) << "0x"
                         << setw((unsigned )ceil(((float)mCacheTagBits)/((float)4))) << hex << mCache[i][j].tag << dec << endl;
                } else if((mCacheWriteHitPolicy == WRITE_THROUGH) || (mCacheWriteMissPolicy == NO_WRITE_ALLOCATE)) {
                    cout << setfill(' ') << setw(7) << i << setw(4) << "0x" << setw((unsigned )ceil(((float)mCacheTagBits)/((float)4))) << hex << mCache[i][j].tag << dec << endl;
                }
            }
        }
    }
}

unsigned cache::evict(unsigned index){
	/* edit here */
    unsigned tempFreeBlock = 0;
    for (unsigned i = 0; i < mCacheAssociativity; i++) {
        if (!mCache[index][i].valid) {
            tempFreeBlock = i;
            return tempFreeBlock;
        }
    }

    tempFreeBlock = LRU_block(index);
    mEvictions++;

    if(mCache[index][tempFreeBlock].dirty)
    {
        //write to memory
        mMemoryWrites++;
    }
    mCache[index][tempFreeBlock].valid = false;
    mCache[index][tempFreeBlock].tag = ~0;
    mCache[index][tempFreeBlock].dirty = false;
    return tempFreeBlock;
}

//implement LRU
unsigned cache::LRU_block(unsigned int index) {
    unsigned tempEvictBlock = 0;
    unsigned LeastTimeStamp= mCache[index][0].timeStamp;
    for (unsigned i = 1; i < mCacheAssociativity; i++) {
        if (mCache[index][i].timeStamp < LeastTimeStamp) {
            LeastTimeStamp = mCache[index][i].timeStamp;
            tempEvictBlock = i;
        }
    }
    return tempEvictBlock;
}

void cache::LRU_update_access(unsigned index, unsigned block){
    mCache[index][block].timeStamp = number_memory_accesses;
}

float cache::get_miss_rate() {
    return ((float)(mReadMisses + mWriteMisses))/ ((float)mRead + (float )mWrite);
}