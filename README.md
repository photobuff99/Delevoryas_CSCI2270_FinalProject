# How to Run
PSA: This is for Unix OS's, Windows support not yet available! Sorry!
    1. Download the project files to your favorite directory
    2. Use makefile to compile files

        cd ~/Downloads
        make

#### Now what?
Well, it depends how you want to test it! I rented an Ubunto server that has
the same operating system (14.04) as our virtual machine, so you can connect
to the board using the client executable you made with make! Since the server
is running a an executable compiled on the virtual machine, we should have
the same version of GCC and the same byte respresentation, so the data
should transfer perfectly!
    1. So, to connect to the remote server I rented, run the client executable
       with 

        ./client 108.61.224.250 9000

       (108.61.224.250 is the IP address of the remote server I rented for $5)
    2. Now try out the server! Type help for help, or help "command goes here"
       to see how to post a new topic, enter a topic, view messages from a topic,
       change topic, or see the current topics!
You may also be wondering, how can I run the server myself? What a great
question! If you want to run the server on your own computer, and connect
using a separate terminal window on your computer or from another computer
on the same LAN:
    1. Start by running the server on a machine

        ./server 9000

    2. Run the client and connect to your server, using the ip address of
       the machine the server is running on.
       Ex: If you're running the server on the same machine as the client,
           enter:

          ./client localhost 9000

       If you're running the server on another computer on the same
       LAN as your client, and the server machine's IP is 192.168.1.3,
       enter: (on the machine you're trying to connect from)

          ./client 192.168.1.3 9000

# Project Summary
This library provides code for a simple LAN message board server, along with
sample client code for accessing and posting to the message board. The message
board consists of a number of different topics, each containing a series of
user-submitted messages. The Client process begins by connecting to the server
specified by the command line arguments. Once connected, the program will
prompt the user for command input, either "ls", "cd", or "post" and set
the current topic to none. If you enter "ls" while the topic = none,
the server will send back a list of topics, and the client will print them
out on your terminal. If you type "cd exampletopicname", the topic will
change to = exampletopicname. If you type "ls" again, the topic name will
print, and if there are any messages currently in that topic, they will display.
If the topic is invalid (not in the messageboard), the topic name will not display,
and the list of messages will not display. If you type "cd" without a topic
specified, the topic will revert to none. If you type "post" while current
topic = none, you will be prompted for a topic name: it is required to be
19 characters long (or less). The topic will then be sent to the server,
the server will create a new topic entry in the hash table, and you can proceed
to enter the topic with "cd topicname". While current topic != none, if
you type "post", you will be prompted to enter a username and a message to post.
The username must be 19 characters or less, and the message must be 139 characters
or less. The message will then be sent to the server, and if you type ls, you
will retrieve the current messages in that topic, including your newly created one.
The server stores up to 10 messages at a time, then it begins deleting the oldest one
as new ones are inserted.
The server is currently running as a daemon process, however if ended, it will
create a binary file containing the topics it had stored in the hash table. If
restarted, it will use the same binary file to reconstruct the messageboard
as it was.

# Dependencies
* This program uses the standard C and C++ libraries, and it is necessary to have access to GCC for compilation.
* Serialization of transmitted bytes has not been implemented, so it is important that systems using the program are running Ubuntu 14.04 and using GCC 4.8.2. However, I have tried connecting from my Mac (running OS X Yosemite 10.10.2 and not using gcc but rather Apple's LLVM version 6.1.0 and clang-602.0.49) and it works just as well as when I connected using the virtual machine.
* This code was written for Unix systems, including Mac OS X and Ubuntu Linux.

# System Requirements
    C/C++ compiler is required to compile the source code.
    Unix systems should be able to run the server and client programs.
    Windows users will not be able to use this library, Solaris users may have issues as well.

# Group Members
    Peter Delevoryas

# Contributors

# Open Issues/Bugs
Issue: very little error checking between processes
Issue: if two servers are running on the same port, client does not indicate there is an issue
Bug: If a user overflows the message posting buffer, it appears to reprint the username in the next post.

# More Information
This simple Server/Client library implements communication between processes, even on different computers, however there is one catch! My program uses the fact that we can transfer bytes across the socket using the read() and write() functions, however unfortunately this will only work on the same computer or very similar computers, as byte representations and compilers will result in different program interpretations, and the data will not be transferred in a standard way. I plan to serialize the data in this program as soon as I can, but since the VM's are all the same, it doesn't really matter, as they all have the same representations of the program. However, it will be impossible to connect with other users using the VM, because the VM uses a NAT to shield itself from the outside world, and without more sophisticated programming methods, there is no way for me to facilitate communication between different VMs. So take my word for it, this program does work across multiple computers! I've tried it with two macs running OS X natively, and the connection was made and the data transferred was correct. Unfortunately, you will likely only be able to simulate this using multiple terminal windows on your single computer.

# Using the program across different computers
If two computers are very similar, this program will work. Otherwise, it will
have trouble transmitting data, but should still be able to connect.
To test it out, run the server on a port like 9000 on one computer,
for example "./server 9000".
Then on another computer, run "./client local_ip_address_of_other_computer 9000"
and the two processes, if successful, will connect, and then you can use the message board!
