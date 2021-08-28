/**************************************************************************
Author      : Manjunath B L
Date        : 22/08/2021
Description : class, structures declarations and library files reside here.
**************************************************************************/

#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iomanip>
#define FAILED_ -1
#define SUCCESS 0
#define ADD 1
#define EDIT 2
#define DELETE 3
#define SUCCESS_MSG "ALL OK"
#define FAILURE_MSG "NOT OK"
#define MSG_SIZE 6 
#define MAX_CONNECTIONS 20
#define MAX_NAME_LENGTH 100
#define DEFAULT_IP "127.0.0.1"

using namespace std;

typedef struct Data
{
    int  choice;
    long number;
    char name[MAX_NAME_LENGTH];
}data;

class SocketServer
{
    public:
        SocketServer(int port=5000):port(port){}
        ~SocketServer(){};
        void  getinfo(){cout<<this->port<<endl;}
        int   CreateSocket();
        int   Bind();
        int   Listen();
        void  Accept();
        void  ReceiveAndClose();

    private:
        int    port;
        int    socket_fd;
        int    data_socket_fd;
        struct sockaddr_in serv_addr;
        data   Data;

};

#endif // SOCKETSERVER_H
