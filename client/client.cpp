/**************************************************************************
Author      : Manjunath B L
Date        : 22/08/2021
Description : All class function  definitions and hadler functions reside here 
**************************************************************************/
#include "client.h"

int SocketClient::CreateSocket()
{
    //create the socket for communication with domain IPv4 Internet protocol and type TCP.
    this->socket_fd=socket(AF_INET, SOCK_STREAM, 0);
    if(this->socket_fd < 0)
        return FAILED_;
    return SUCCESS;
}
int SocketClient::Bind()
{
    //populate the sockaddr structure with proper domain,type and IP for connecting
    //to server
    char* IP= new char[sizeof(this->address.c_str())];
    strcpy(IP,this->address.c_str());
    this->ServerInfo.sin_family = AF_INET;
    this->ServerInfo.sin_addr.s_addr = inet_addr(IP);
    this->ServerInfo.sin_port = htons(this->port);
    delete IP;
    return SUCCESS;
}
int SocketClient::Connect()
{
    //connect is a blocking call and returns after three way hand shake completion else returns -1
    if(connect(socket_fd, (struct sockaddr *)&this->ServerInfo, sizeof(this->ServerInfo)) != 0)
    {
        cout << "[ERROR]: Error connecting to server"<< endl;
        return FAILED_;
    }
    cout<<"[INFO]: Connection successfull"<< endl;

    return SUCCESS;
}
int SocketClient::SendAndClose()
{
    //send data according to user needs.
    switch(this->Data.choice)
    {
        //add user data to server.
        case ADD:

            if(send(this->socket_fd, (data*)&(this->Data),sizeof(data),0))
                cout<<"[INFO]: DATA sent to server successsfully"<<endl;
            else
                return FAILED_;

            recv(this->socket_fd,(void*)this->ServerMsg,SERVER_MSG_SIZE,0);
            if(!strcmp(this->ServerMsg,FAILURE_MSG))
                return FAILED_;
            else
                cout<<"[SERVER INFO]: ADD SUCCESS"<<endl;
            break;

        //edit user data in server.
        case EDIT:

            //First check id data exits in server else return error.
            if(send(this->socket_fd, (data*)&(this->Data),sizeof(data),0))
                cout<<"[INFO]: DATA sent to server successsfully"<<endl;
            else
                return FAILED_;
            if(HandleEdit() == FAILED_)
            {
                cout<<"[SERVER ERROR]: No such name exists"<<endl;
                return FAILED_;
            }
            else
                cout<<"[SERVER INFO]: EDIT SUCCESS"<<endl;
            break;
        //delete user data in server.
        case DELETE:

            //check if data exist in server to delete else return error.
            if(send(this->socket_fd, (data*)&(this->Data),sizeof(data),0))
                cout<<"[INFO]: DATA sent to server successsfully"<<endl;
            else
                return FAILED_;

            recv(this->socket_fd,(void*)this->ServerMsg,SERVER_MSG_SIZE,0);

            if(!strcmp(this->ServerMsg,FAILURE_MSG))
                cout<<"[SERVER ERROR]: No such name exists"<<endl;
            else
                cout<<"[SERVER INFO]: DELETE SUCCESS"<<endl;
            break;

    }
    //close the socket and return.
    close(socket_fd);
    return SUCCESS;
}
int SocketClient::DisplayMenu()
{
    int choice{0};
    cout<<" ---------------------"<<endl;
    cout<<"|Welcome to the server|"<<endl;
    cout<<" ---------------------"<<endl;
    cout<<"Choose any of the following options"<<endl<<endl;
    cout<<"1:ADD    contact"<<endl;
    cout<<"2:EDIT   contact"<<endl;
    cout<<"3:DELETE contact"<<endl<<endl;
    cout<<"Enter the opition:";
    cin>>choice;
    switch(choice)
    {
        case 1:
            cout<<"Enter the name and number to add:"<<endl;
            cout<<"NAME>>";
            cin>>this->Data.name;
            cout<<"NUMBER>>";
            cin>>this->Data.number;
            this->Data.choice=choice;
            break;
        case 2:
            cout<<"Enter the contact name to edit:"<<endl;
            cout<<"NAME>>";
            cin>>this->Data.name;
            this->Data.choice=choice;
            break;
        case 3:
            cout<<"Enter the contact name to delete:"<<endl;
            cout<<"NAME>>";
            cin>>this->Data.name;
            this->Data.choice=choice;
            break;
        default:
            cout<<"wrong option"<<endl;
    }
    return choice;

}
int SocketClient::HandleEdit()
{
    recv(this->socket_fd,(void*)this->ServerMsg,SERVER_MSG_SIZE,0);
    if(!strcmp(this->ServerMsg,FAILURE_MSG))
    {
        cout<<"[ERROR]: coudn't find the name to edit"<<endl;
        return FAILED_;
    }
    else
    {
        cout<<"Enter the new name:";
        cin>>this->Data.name;
        if(send(this->socket_fd,(data*)&this->Data,sizeof(Data),0))
            cout<<"[INFO]: DATA sent to server successsfully"<<endl;
        else
            return FAILED_;
        recv(this->socket_fd,(void*)this->ServerMsg,SERVER_MSG_SIZE,0);
        if(!strcmp(this->ServerMsg,FAILURE_MSG))
        {
            return FAILED_;
        }
    }
    return SUCCESS;
}

//end of file :)
