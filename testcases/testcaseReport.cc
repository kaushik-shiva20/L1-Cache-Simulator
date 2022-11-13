#include "cache.h"
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <cstring>
#include <string>
#include <iomanip>
#include <map>

#define KB 1024

#define NUM_CACHE_SIZE_CONFIGS 3
#define NUM_CACHE_ASSOC_CONFIGS 4
#define NUM_CACHE_BLOCK_SIZE_CONFIGS 4

using namespace std;

/* Test case for cache simulator */ 

/* DO NOT MODIFY */
typedef struct
{
    unsigned size;
    unsigned associativity;
    unsigned line_size;
    write_policy_t wr_hit_policy;
    write_policy_t wr_miss_policy;
    unsigned hit_time;
    unsigned miss_penalty;
    unsigned address_width;
}mCache_Config_t;
int main(int argc, char **argv){

    const char * mFileName[2] = {"traces/matrix_out.t", "traces/block_matrix_out.t"};
    unsigned mFileNameIndex = 0;
    if(argc < 2)
    {
        cout << "Error: Usage - ./testcaseReport multiply or ./testcaseReport block_multiply"<< endl;
        return -1;
    }
    if (!strcmp(argv[1], "multiply")){
        mFileNameIndex = 0;
    }
    else if (!strcmp(argv[1], "block_multiply")){
        mFileNameIndex = 1;
    }
    else{
        cout << "Error: Usage - ./testcaseReport multiply or ./testcaseReport block_multiply"<< endl;
        return -1;
    }
    ofstream report_file;
    ofstream cache_config;
    report_file.open("report.txt");
    //cache_config.open("cache_config.txt");
    if(!report_file.is_open()){
        cout << "Report file could not be opened" << endl;
        return -1;
    }

    report_file << setfill(' ') << setw(9) << "SIZE(KB)" << setw(15) << "ASSOCIATIVITY"
    << setw(13) << "BLOCKSIZE(B)" << setw(17) << "WRITE_HIT_POLICY" << setw(18) << "WRITE_MISS_POLICY"
    << setw(10) << "MISS_RATE" << endl;
    //cache_config << "Cache Size " << "Block Size " << "Associativity " << "#Blocks " << "#Sets " << "#Offset Bits " << "#Index Bits " << "#Tag Bits" << endl;
    unsigned mCacheSizeConfig[NUM_CACHE_SIZE_CONFIGS] = {16*KB, 32*KB, 64*KB};
    unsigned mAssociativityConfig[NUM_CACHE_ASSOC_CONFIGS] = {1, 2, 4, 16};
    unsigned mBlockSizeConfig[NUM_CACHE_BLOCK_SIZE_CONFIGS] = {32, 64, 128, 256};
    write_policy_t mWriteHitPolicyConfig[2] = {WRITE_BACK, WRITE_THROUGH};
    write_policy_t mWriteMissPolicyConfig[2] = {WRITE_ALLOCATE, NO_WRITE_ALLOCATE};
    string mWriteHitPolicyConfigStr[2] = {"WRITE_BACK", "WRITE_THROUGH"};
    string mWriteMissPolicyConfigStr[2] = {"WRITE_ALLOCATE", "NO_WRITE_ALLOCATE"};

    mCache_Config_t mCacheConfig;
    mCacheConfig.hit_time = 1;
    mCacheConfig.miss_penalty = 1;
    mCacheConfig.address_width = 32;
    for(unsigned currCachePolicy = 0; currCachePolicy < 2; currCachePolicy++){
        mCacheConfig.wr_hit_policy = mWriteHitPolicyConfig[currCachePolicy];
        mCacheConfig.wr_miss_policy = mWriteMissPolicyConfig[currCachePolicy];
        for(unsigned currCacheSize = 0; currCacheSize < NUM_CACHE_SIZE_CONFIGS; currCacheSize++){
            mCacheConfig.size = mCacheSizeConfig[currCacheSize];
            for(unsigned currBlockSize = 0; currBlockSize < NUM_CACHE_BLOCK_SIZE_CONFIGS; currBlockSize++){
                mCacheConfig.line_size = mBlockSizeConfig[currBlockSize];
            for(unsigned currAssociativity = 0; currAssociativity < NUM_CACHE_ASSOC_CONFIGS; currAssociativity++){
                mCacheConfig.associativity = mAssociativityConfig[currAssociativity];
                    cache *mycache = new cache(mCacheConfig.size,
                                               mCacheConfig.associativity,
                                               mCacheConfig.line_size,
                                               mCacheConfig.wr_hit_policy,
                                               mCacheConfig.wr_miss_policy,
                                               mCacheConfig.hit_time,
                                               mCacheConfig.miss_penalty,
                                               mCacheConfig.address_width);

                    //cache_config << setfill(' ') << setw(10) << mCacheConfig.size/KB << setw(10) << mCacheConfig.line_size << setw(13) << mCacheConfig.associativity << setw(7) << mycache->mCacheSets << setw(5) << mycache->mCacheSets << setw(11) << mycache->mCacheLineOffsetBits << setw(10) << mycache->mCacheSetIndexBits << setw(8) << mycache->mCacheTagBits << endl;
                    mycache->load_trace(mFileName[mFileNameIndex]);
                    mycache->run();
                    report_file << setfill(' ') << setw(8) << mCacheConfig.size/KB << setw(15) << mCacheConfig.associativity
                    << setw(13) << mCacheConfig.line_size << setw(17) << mWriteHitPolicyConfigStr[currCachePolicy]
                    << setw(18) << mWriteMissPolicyConfigStr[currCachePolicy] << " " << setw(10) << fixed << setprecision(4) << (mycache->get_miss_rate()) << endl;
                    delete mycache;
                }
            }
        }
    }
    report_file.close();
    //cache_config.close();
}

