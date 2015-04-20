#include "Utilities.h"
#include <iostream>
#include <fstream>

using std::cout; using std::cin;
using std::string; using std::cerr;
using std::endl;

void RequestMessage(int sockfd);
void sendMessage(string message);
string recvMessage();

int main()
{
    int sockfd;

    sockfd = Socket("localhost", SERV_PORT, AF_UNSPEC, SOCK_STREAM, 0);
    if (sockfd == -1) {
        cerr << "Error on socket()" << endl;
        exit(1);
    }


    RequestMessage(sockfd);
    close(sockfd);
    return 0;
}

void RequestMessage(int sockfd)
{
    string s = RecvMessage(sockfd, 256);
    cout << "Received:\n" << s << endl;
    SendMessage(sockfd, "Hello Server!");
}
