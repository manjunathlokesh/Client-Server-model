/**********************************************************************************
Author      : Manjunath B L
Date        : 28/08/2021
Description : All class function  definitions and hadler functions reside here 
 **********************************************************************************/

#include "shared_mem.h"

int SharedMemory::CreateSharedMemory()
{
    //shmget creates shared memory of specified key if it exits it returns +ve value.
    //more info on man shmget.
    this->shmid=shmget (0x1234,sizeof(struct user_data), 0644|IPC_CREAT);
    if(this->shmid == -1)
        return FAILED_;
    else
        return SUCCESS;
}
int SharedMemory::AttachToMemory()
{
    //shmat attaches the structure we specify to the shared memory of specified key.
    this->data=(UserData*)shmat (this->shmid, 0, 0);
    if(this->data == NULL)
        return FAILED_;
    else
        return SUCCESS;
}
int SharedMemory::AddData(struct Data data)
{
    //populating data into the structure.
    pthread_mutex_lock(&lock);
    strcpy(this->data->name[this->data->synnum],data.name);
    this->data->number[this->data->synnum]=data.number;
    this->data->synnum++;
    pthread_mutex_unlock(&lock);
    return SUCCESS;
}
int SharedMemory::FindData(struct Data data,int &position)
{
    //finding data in database.
    pthread_mutex_lock(&lock);
    for(int i=0;i<MAX_MEM_SIZE;i++)
    {
        if(!(strcmp(this->data->name[i],data.name)))
        {
            position=i;
            pthread_mutex_unlock(&lock);
            return SUCCESS;
        }
     }
    pthread_mutex_unlock(&lock);
    return FAILED_;
}
int SharedMemory::EditData(struct Data data,int position)
{
    //change the old name to new name in position found by find.
    pthread_mutex_lock(&lock);
    strcpy(this->data->name[position],data.name);
    pthread_mutex_unlock(&lock);
    return SUCCESS;
}

int SharedMemory::DeleteData(struct Data data)
{
    //find the data to delete and also adjust the space so that there are no leakage.
    //this is an O(n^2) solution need to improve it future to atleast O(n).
    pthread_mutex_lock(&lock);
    for(int i=0;i<MAX_MEM_SIZE;i++)
    {
        if(!(strcmp(this->data->name[i],data.name)))
        {
            for(int j=i;j<MAX_MEM_SIZE-1;j++)
            {
                strcpy(this->data->name[j],this->data->name[j+1]);
                this->data->number[j]=this->data->number[j+1];
            }
            this->data->synnum--;
            return SUCCESS;
        }
    }
    pthread_mutex_unlock(&lock);
    return FAILED_;
}

int SharedMemory::DetachFromMemory()
{
    //detach from the shared memory.
    shmdt (this->data);
    return SUCCESS;
}
void SharedMemory::CheckData()
{
    //Displaying the contect list after processing from client.
    cout<<endl;
    cout<<"------------------------------"<<endl;
    cout<<"|        contact list        |"<<endl;
    cout<<"------------------------------"<<endl;

    for(int i=0;i<MAX_MEM_SIZE;i++)
    {
        if(this->data->number[i] == 0)
            break;
        cout<<std::left<<std::setw(20);
        cout<<this->data->name[i];
        cout<<std::left<<std::setw(10);
        cout<<this->data->number[i]<<endl;
    }
    cout<<"------------------------------"<<endl;
    cout<<endl;
}

//end of file :)
