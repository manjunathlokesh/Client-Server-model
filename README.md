# Client-Server-model
A Linux based distributed application comprising server, client and its very own database. Giving client features like adding, deleting and modifying data in server database
This is achieved employing Linux socket programming(uses TCP/IP protocol suite) and shared memory IPC.
# Software requirments:
Linux OS(V 16.04 or up) running on the host machine or in a virtual machine with latest **gcc/g++ compilers** installed.
# Compilation notes:
Open two terminals and navigate to the respective server and client directory and compile the programs using the following commands.

**For server**
```sh
g++ -g -Wall -std=c++14 shared_mem.cpp server_main.cpp server.cpp -o server -lpthread
```
**For client**
```sh
g++ -g -Wall -std=c++14 client_main.cpp client.cpp -o client
```
## Running the program:
**server side:**
```sh
./server
```
**client side:**
```sh
./client
```
**packet analysis:**
```sh
sudo tcpdump -w text.pcap -i lo
```
*Please note:* run server and packet analizer first before running client, and press ctrl-c to exit form server and packet sniffer.
## Expected output:
Please refer to **OutputImages folder** in the repo.
