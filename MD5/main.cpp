//
//  main.cpp
//  MD5
//
//  Created by 孙 嘉裕 on 12-12-29.
//  Copyright (c) 2012年 孙 嘉裕. All rights reserved.
//

#include "CommonHeaders.h"
#include "enum.h"

#define HOST "192.168.1.102"
#define PORT 6001


class Worker{
	char *CHARSET;
	unsigned CHARSET_LENGTH;
	long long start,end;
	int sock;
	static mutex locker;
	void Recv(char* dest,long size){
		long rec_size=0;
		while(size && (rec_size=recv(sock, dest, size, 0))>0){
			size-=rec_size;
		}
		if(size)
			throw "recv Error.";
	}
	void Send(char *source,long size){
		long sent=0;
		while(size && (sent=send(sock, source, size, 0))>0){
			source+=sent;
			size-=sent;
		}
		if(size)
			throw "send Error.";
	}
public:
	void main(){
		while (true) {
			Recv((char*)&start, sizeof(long long));
			Recv((char*)&end, sizeof(long long));
			Recv((char*)&CHARSET_LENGTH, sizeof(unsigned));
			CHARSET=new char[CHARSET_LENGTH];
			Recv(CHARSET, CHARSET_LENGTH);
			auto result = Enum(start, end, CHARSET, CHARSET_LENGTH);
			Send((char*)result, (end-start)*sizeof(pair<uint64_t, uint64_t>));
		}
	}
	static void startWork(Worker *worker){
		try{
			struct sockaddr_in pin;
			memset(&pin,0,sizeof(pin));
			pin.sin_family=AF_INET;
			inet_pton(AF_INET,HOST,&pin.sin_addr);
			pin.sin_port=htons(PORT);
			if((worker->sock=socket(PF_INET,SOCK_STREAM,IPPROTO_IP))==-1)
			{
				throw "Error openung socket!";
			}
			if(connect(worker->sock,(struct sockaddr*)&pin,sizeof (pin))==-1)
			{
				throw "can not connecting to server!";
			}
			worker->main();
		}catch(const char* e){
			locker.lock();
			cout<<e<<endl;
			locker.unlock();
		}
	}
};

mutex Worker::locker;

int main(int argc, const char * argv[])
{
	long num = sysconf(_SC_NPROCESSORS_ONLN);
	vector<thread*> worker_t;
	for(long i=0;i<num;i++){
		Worker *worker=new Worker;
		worker_t.push_back(new thread(worker->startWork,worker));
	}
	for_each(worker_t.begin(), worker_t.end(), [](thread *t){
		t->join();
	});
    return 0;
}

