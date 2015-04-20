#include <iostream>
#include "Utilities.h"
#include <string>
#include <fstream>

using std::cout; using std::cin;
using std::string; using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;

void HandleClient(int clientfd);

void WriteMessage(string filename, string message);
string GetMessages(string filename);

int main(int argc, char*argv[]) {
    int servfd, clientfd;
    socklen_t sin_size;
    struct sockaddr_storage their_addr;


    servfd = Socket(NULL, SERV_PORT, AF_UNSPEC, SOCK_STREAM, 0);
    if (servfd == -1) {
        cerr << "Error getting file descriptor" << endl;
        exit(1);
    }
    if (Listen(servfd, 10) == -1) {
        cerr << "Error listening" << endl;
        exit(1);
    }


    while (1) {
        sin_size = sizeof their_addr;
        clientfd = accept(servfd, (struct sockaddr *)&their_addr, &sin_size);
        if (clientfd == -1) {
            cerr << "Error accepting connection" << endl;
            continue;
        }

        if (!fork()) { // child process
            close(servfd);
            HandleClient(clientfd);
            close(clientfd);
            exit(0);
        }
        close(clientfd);
    }

    close(servfd);
    return 0;
}

void HandleClient(int clientfd)
{
    //SendMessage(clientfd, "Hello, client!");
    string msg = GetMessages("test.txt");
    SendMessage(clientfd, msg);
    string recvd = RecvMessage(clientfd, 256);
    cout << "server: received " << recvd << endl;
    WriteMessage("test.txt", recvd);
}

void WriteMessage(string filename, string message)
{
    ofstream out (filename, std::ios::app);
    if (out.is_open()) {
        out << message;
        out << endl;
        out.close();
    }
}

string GetMessages(string filename)
{
    std::ifstream in (filename);
    std::string messages, line;
    while (getline(in,line)) {
        messages += line;
        messages += "\n";
    }
    return messages;
}
