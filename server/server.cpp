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

void SocketServer::Accept ()
{
    int pid;
    cout<<"[INFO]: Server is Running"<<endl;
    while (1)
    {
        //accept is a blocking call it will accept the connection and wait for other connections
        //return from accept means three way hand shake is completed i.e SYN,SYN-ACK,ACK.
        this->data_socket_fd = accept (this->socket_fd, (struct sockaddr *) NULL, NULL);

        if (this->data_socket_fd > 0)
            cout << "[INFO]: Connection accepted with ID: " << this->data_socket_fd << endl;
        else
            cout << "[ERROR]: Connection Not accepted\n" << endl;

        pid = fork ();

        //child process handeles the current connection while parent process waits for new connections.
        if (pid == 0)
        {
            //Sharedmemory class object is responsible for handling API's of our database.
            SharedMemory DataBase;

            //recv function reads from data socket file discriptor and populates it to structure of type data.
            recv(this->data_socket_fd,(data *)&(this->Data),sizeof(data), 0);

            switch (this->Data.choice)
            {

                //switch accoding to client requested functionality.
                case ADD:
                    {

                        //Add client data to database and ackknowledge client.
                        if(AddToDataBase(DataBase,this->Data) == FAILED_)
                            send(this->data_socket_fd,(void*)FAILURE_MSG,MSG_SIZE,0);
                        else
                            send(this->data_socket_fd,(void*)SUCCESS_MSG,MSG_SIZE,0);
                        break;
                    }
                case EDIT:
                    {

                        int position{0};
                        
                        //First find the data is available or not to edit.
                        if(FindData(DataBase,this->Data,position) == FAILED_)
                        {
                            send(this->data_socket_fd,(void*)FAILURE_MSG,MSG_SIZE,0);
                        }
                        else
                        {
                            //If data is found again receive the data to edit.
                            send(this->data_socket_fd,(void*)SUCCESS_MSG,MSG_SIZE,0);
                            recv(this->data_socket_fd,(data *)&(this->Data),sizeof(data),0);

                            //send acknowledgment to client whether edit success or failure.
                            if(EditDataBase(DataBase,this->Data,position) == FAILED_)
                            {

                                send(this->data_socket_fd,(void*)FAILURE_MSG,MSG_SIZE,0);
                            }
                            else
                                send(this->data_socket_fd,(void*)SUCCESS_MSG,MSG_SIZE,0);
                        }
                        break;
                    }
                case DELETE:
                    {
                        //delete the data requested by the client.
                        if(DeleteFromDatabase(DataBase,this->Data) == FAILED_)
                            send(this->data_socket_fd,(void*)FAILURE_MSG,MSG_SIZE,0);
                        else
                            send(this->data_socket_fd,(void*)SUCCESS_MSG,MSG_SIZE,0);
                        break;
                    }
                default:
                    cout<<"[ERROR]: Invalid choice"<<endl;

            }
            //close the socket of the running client.
            close (this->data_socket_fd);
            //exit from the child.
            exit (0);
        }
    }
}
int AddToDataBase(SharedMemory &DataBase,data Data)
{
    //create shared memory if exists open it.
    if (DataBase.CreateSharedMemory () == FAILED_)
        cout << "[INFO]: Shared memory already exists" << endl;

    //create semaphore object if exist open it.
     if (DataBase.CreateSemaphore() == FAILED_)
        cout << "[INFO]: semaphore already exists" << endl;

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

    if (DataBase.CreateSemaphore() == FAILED_)
        cout << "[INFO]: semaphore already exists" << endl;

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

    if (DataBase.CreateSemaphore() == FAILED_)
        cout << "[INFO]: semaphore already exists" << endl;

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

    if (DataBase.CreateSharedMemory () == FAILED_)
        cout << "[INFO]: Shared memory already exists" << endl;

    if (DataBase.CreateSemaphore() == FAILED_)
        cout << "[INFO]: semaphore already exists" << endl;
    if (DataBase.AttachToMemory () == FAILED_)
    {
        cout << "[ERROR]: Attaching to shared memory failed" <<endl;
        return FAILED_;
    }
    if (DataBase.DeleteData (Data) == SUCCESS)
    {
        cout << "[INFO]: Contact list after deleteion" << endl;
        DataBase.CheckData ();
    }
    else
    {
        if (DataBase.DetachFromMemory () == FAILED_)
            cout <<"[INFO]: Detaching from the shared memory Failed" <<endl;
        return FAILED_;
    }
    if (DataBase.DetachFromMemory () == FAILED_)
        cout <<"[INFO]: Detaching from the shared memory Failed" <<endl;
    return SUCCESS;
}

//end of file :)
