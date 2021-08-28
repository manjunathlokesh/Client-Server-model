/**************************************************************************
Author      : Manjunath B L
Date        : 22/08/2021
Description : Driver function for the clinet responsibl for connecting to server,
              and data transfer.
**************************************************************************/

#include"client.h"

int main()
{
    SocketClient soc;                      //can also pass IP addressport number EX:SocketClient("192.168.172.1",3000)

    if(soc.CreateSocket() == FAILED_)      //creates an endpoint for communication
    {
        perror ("[ERROR]: ");
        exit(EXIT_FAILURE);
    }

    if(soc.Bind() == FAILED_)             //assining(binding) destination ip address,
    {                                     //port no and specifing protocol to use.
        perror ("[ERROR]: ");
        exit(EXIT_FAILURE);
    }

    if(soc.Connect() == FAILED_)         //connect to server of specified info.
    { 
        perror ("[ERROR]: ");
        exit(EXIT_FAILURE);
    }

    soc.DisplayMenu();                  //Display available services from the server

    if(soc.SendAndClose() == FAILED_)   //comunicate with client and close the connection.
    { 
        perror ("[ERROR]: ");
        exit(EXIT_FAILURE);
    }

    return 0;
}

//end of file :)
