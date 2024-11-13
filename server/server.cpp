/**********************************************************************************
Author      : Manjunath B L
Date        : 22/08/2021
Description : All class function  definitions and hadler functions reside here 
**********************************************************************************/

#include "server.h"
#include "shared_mem.h"

//Functions prototype's of helper functions to class functions

int AddToDataBase(SharedMemory &,data);
int DeleteFromDatabase(SharedMemory &,data);
int FindData(SharedMemory &,data,int &position);
int EditDataBase(SharedMemory &,data,int position);

int SocketServer::CreateSocket ()
{
    //create the socket for communication with domain IPv4 Internet protocol and type TCP.
    this->socket_fd=socket (AF_INET, SOCK_STREAM, 0);
    if (this->socket_fd < 0)
        return FAILED_;
    return SUCCESS;
}

int SocketServer::Bind ()
{
    //populate the sockaddr structure with proper domain,type and IP for accepting connections
    //of this context.
    this->serv_addr.sin_family = AF_INET;             //IPV4
    this->serv_addr.sin_addr.s_addr = INADDR_ANY;    //accept form any IP
    this->serv_addr.sin_port = htons (this->port);  //port number

    return bind(this->socket_fd, (struct sockaddr *) &this->serv_addr, sizeof (this->serv_addr));
}

int SocketServer::Listen ()
{
    //Listen puts the server socket in a passive mode, where it waits for the client to approach 
    //the server to make a connection.
    if (listen (this->socket_fd, MAX_CONNECTIONS) < 0)
        return FAILED_;
    return SUCCESS;
}
void* SocketServer::connection_handler(void* con_obj)
{
    SharedMemory DataBase;
    connections *obj=static_cast<connections*>(con_obj);
    //recv function reads from data socket file discriptor and populates it to structure of type data.
    recv(obj->data_socket_fd,(data *)&(obj->Data),sizeof(data), 0);
    cout<<obj->data_socket_fd<<obj->Data.choice<<endl;
    switch (obj->Data.choice)
    {
        //switch accoding to client requested functionality.
        case ADD:
            {
                //Add client data to database and ackknowledge client.
                if(AddToDataBase(DataBase,obj->Data) == FAILED_)
                    send(obj->data_socket_fd,(void*)FAILURE_MSG,MSG_SIZE,0);
                else
                    send(obj->data_socket_fd,(void*)SUCCESS_MSG,MSG_SIZE,0);
                break;
            }
        case EDIT:
            {

                int position{0};

				//First find the data is available or not to edit.
				if(FindData(DataBase,obj->Data,position) == FAILED_)
				{
					send(obj->data_socket_fd,(void*)FAILURE_MSG,MSG_SIZE,0);
				}
				else
				{
					//If data is found again receive the data to edit.
					send(obj->data_socket_fd,(void*)SUCCESS_MSG,MSG_SIZE,0);
					recv(obj->data_socket_fd,(data *)&(obj->Data),sizeof(data),0);

					//send acknowledgment to client whether edit success or failure.
					if(EditDataBase(DataBase,obj->Data,position) == FAILED_)
					{

						send(obj->data_socket_fd,(void*)FAILURE_MSG,MSG_SIZE,0);
					}
					else
						send(obj->data_socket_fd,(void*)SUCCESS_MSG,MSG_SIZE,0);
				}
				break;
            }
        case DELETE:
            {
                //delete the data requested by the client.
                if(DeleteFromDatabase(DataBase,obj->Data) == FAILED_)
                    send(obj->data_socket_fd,(void*)FAILURE_MSG,MSG_SIZE,0);
                else
                    send(obj->data_socket_fd,(void*)SUCCESS_MSG,MSG_SIZE,0);
                break;
            }
        default:
            cout<<"[ERROR]: Invalid choice "<<obj->data_socket_fd<<endl;

    }
    close (obj->data_socket_fd);
    return NULL;

}
void SocketServer::create_worker_thread(int connection_count)
{
    if (pthread_create(&this->connection_threads[this->connection_count], NULL,SocketServer::connection_handler,&this->user_data[connection_count]) != 0)
        cout<<"[ERROR]: failed to create worker thread"<<endl;
    if(this->connection_count > 99)
    {
        for(int i=0;i<100;i++)
            pthread_join(this->connection_threads[i],NULL);
    }
}

void SocketServer::Accept ()
{
    cout<<"[INFO]: Server is Running"<<endl;
    while (1)
    {
        //accept is a blocking call it will accept the connection and wait for other connections
        //return from accept means three way hand shake is completed i.e SYN,SYN-ACK,ACK.
        this->user_data[connection_count].data_socket_fd= accept (this->socket_fd, (struct sockaddr *) NULL, NULL);

        if (this->user_data[this->connection_count].data_socket_fd > 0)
            cout << "[INFO]: Connection accepted with ID: " << this->user_data[this->connection_count].data_socket_fd << endl;
        else
            cout << "[ERROR]: Connection Not accepted\n" << endl;
        create_worker_thread(this->connection_count);
        this->connection_count++;
        if(this->connection_count >= 99)
        {
            cout << "[INFO]: MAX connection limit reached" << endl;
            break;
        }

    }
}
int AddToDataBase(SharedMemory &DataBase,data Data)
{
    //create shared memory if exists open it.
    if (DataBase.CreateSharedMemory () == FAILED_)
        cout << "[INFO]: Shared memory already exists" << endl;

    //attach to the shared memory to append client data.
    if (DataBase.AttachToMemory () == FAILED_)
    {
        cout << "[ERROR]: Attaching to shared memory failed" <<endl;
        return FAILED_;
    }

    //Add data to our data base and check status.
    if (DataBase.AddData (Data) == SUCCESS)
    {
        cout << "[INFO]: Contact list after addition" << endl;
        //CheckDtata is employed to verify whether data is added properly
        //can also comment this if you wish.
        DataBase.CheckData();
    }
    else
        return FAILED_;

    //detach from the shared memory which we are using as database.
    if (DataBase.DetachFromMemory () == FAILED_)
    {
        cout << "[INFO]: Detaching from the shared memory Failed" <<endl;
        return FAILED_;
    }

    return SUCCESS;
}
int FindData(SharedMemory &DataBase,data Data,int &position)
{

    if (DataBase.CreateSharedMemory() == FAILED_)
        cout << "[INFO]: Shared memory already exists" << endl;

    if (DataBase.AttachToMemory() == FAILED_)
    {
        cout << "[ERROR]: Attaching to shared memory failed" <<endl;
        return FAILED_;
    }

    //Here we are finding the data to edit and storing its position
    //in memory so that after receving new data to edit we dont need to search again.
    if (DataBase.FindData(Data,position) == SUCCESS)
        return SUCCESS;
    if (DataBase.DetachFromMemory() == FAILED_)
        cout <<"[INFO]: Detaching from the shared memory Failed" <<endl;
    return FAILED_;


}
int EditDataBase(SharedMemory &DataBase,data Data,int position)
{

    if (DataBase.CreateSharedMemory() == FAILED_)
        cout << "[INFO]: Shared memory already exists" << endl;

    if (DataBase.AttachToMemory() == FAILED_)
    {
        cout << "[ERROR]: Attaching to shared memory failed" <<endl;
        return FAILED_;
    }

    if (DataBase.EditData(Data,position) == SUCCESS)
    {
        cout << "[INFO]: Contact list after Edit" << endl;
        DataBase.CheckData();
    }
    else
        return FAILED_;
    if (DataBase.DetachFromMemory() == FAILED_)
        cout <<"[INFO]: Detaching from the shared memory Failed" <<endl;
    return SUCCESS;
}
int DeleteFromDatabase(SharedMemory &DataBase,data Data)
{
    cout <<"[ERROR] entered delete data" <<endl;

    if (DataBase.CreateSharedMemory () == FAILED_)
        cout << "[INFO]: Shared memory already exists" << endl;

    if (DataBase.AttachToMemory () == FAILED_)
    {
        cout << "[ERROR]: Attaching to shared memory failed" <<endl;
        return FAILED_;
    }
    cout <<"[ERROR] before entering shared memeory function" <<endl;
    if (DataBase.DeleteData (Data) == SUCCESS)
    {
        cout << "[INFO]: Contact list after deleteion" << endl;
        DataBase.CheckData ();
    }
    else
    {
        if (DataBase.DetachFromMemory () == FAILED_)
        {
            cout <<"[INFO]: Detaching from the shared memory Failed" <<endl;
        }

        cout <<"[ERROR]: DELETE DATA FAILED" <<endl;
        return FAILED_;
    }
    if (DataBase.DetachFromMemory () == FAILED_)
        cout <<"[INFO]: Detaching from the shared memory Failed" <<endl;
    return SUCCESS;
}

//end of file :)
