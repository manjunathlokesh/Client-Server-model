/**************************************************************************
Author      : Manjunath B L
Date        : 22/08/2021
Description : class, structures declarations and library files reside here.
**************************************************************************/

#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <sys/shm.h>
#include <cstring>
#include "server.h"
#define MAX_MEM_SIZE 100
#define FAILED_ -1
#define SUCCESS 0
using namespace std;

typedef struct user_data{

    //only static memory allocations are allowed in shared memory
    //assigning pointers to shared memory is a mess.
    long number[MAX_MEM_SIZE];
    char name[MAX_MEM_SIZE][100];
    int  synnum;

}UserData;

class SharedMemory
{
    public:
        SharedMemory():data(nullptr){};
        int CreateSharedMemory();
        int AttachToMemory();
        int AddData(struct Data data);
        int FindData(struct Data data,int &position);
        int EditData(struct Data data,int position);
        int DeleteData(struct Data data);
        int DetachFromMemory();
        void CheckData();
    private:
        int shmid;
        UserData *data;

};

#endif //SHAREDMEMORY_H
