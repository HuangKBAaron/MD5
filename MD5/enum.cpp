//
//  enum.cpp
//  MD5
//
//  Created by 孙 嘉裕 on 12-12-29.
//  Copyright (c) 2012年 孙 嘉裕. All rights reserved.
//

#include "enum.h"
#include "MD5.h"
void (*pfunc)(char *);
pair<uint64_t, uint64_t>* Enum(int64_t start,int64_t end,char* CHARSET,unsigned int CHARSET_LENGTH){
	if(end<=start)
		return NULL;
	uint8_t str[64];
	int idx[64];
	int len=0;
	pair<uint64_t, uint64_t> *ret=new pair<uint64_t, uint64_t>[end-start];
	
	//Get the first string
	for(int64_t i=start;i;i/=CHARSET_LENGTH){
		str[len]=CHARSET[idx[len]=--i%CHARSET_LENGTH];
		++len;
	}
	str[len]=0x80;
	for(int i=len+1;i<64;i++)
		str[i]=0;
	*(int*)&str[64-8]=len<<3;
	
	for(int64_t curIdx=start;curIdx!=end;++curIdx){
		
		ret[curIdx-start]=md5((uint32_t*)str);
		
		for(int i=0;i<len;i++){
			if(idx[i]==CHARSET_LENGTH-1){
				idx[i]=0;
				str[i]=CHARSET[0];
			}else{
				str[i]=CHARSET[++idx[i]];
				goto Done;
			}
		}
		idx[len]=0;
		str[len]=CHARSET[0];
		++len;
		str[len]=0x80;
		*(int*)&str[64-8]=len<<3;
	Done:;
	}
	return ret;
}