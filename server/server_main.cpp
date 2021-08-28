/**********************************************************************************
Author      : Manjunath B L
Date        : 22/08/2021
Description : This is the Driver function which is responsible bringing our server
              up and initalizing database and accept-handle multiple connections.
**********************************************************************************/

#include"server.h"

//Program starts here.
int main()
{

    SocketServer soc;                    //can also pass port number EX:socketServer(3000)

    if(soc.CreateSocket() == FAILED_)    //creates an endpoint for communication
    {
        perror("[ERROR]");
        exit(EXIT_FAILURE);
    }

    if(soc.Bind() == FAILED_)           //assining(binding) destination ip address,
    {                                   //port no and specifing protocol to use.
        perror("[ERROR]");
        exit(EXIT_FAILURE);
    }

    if(soc.Listen() == FAILED_)         //make the server to start looking for
    {                                   //incomming connections
        perror("[ERROR]");
        exit(EXIT_FAILURE);
    }

    soc.Accept();                      //Start accepting incomming connections and process

    //program never reaches here
    return 0;
}  
