# How to Run
PSA: This is for Unix OS's, Windows support not yet available! Sorry!
    1. Download the project files to your favorite directory
    2. Use makefile to compile files

        cd ~/Downloads
        make

#### Now what?
Well, it depends how you want to test it! I rented an Ubuntu server that has
the same operating system (14.04) as our virtual machine (for only $5! what a deal!!), so you can connect
to the server I'm running on the remote virtual machine I rented using the client executable you made with make! Since the server
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
This library provides code for a simple messageboard server and client. The messageboard is structured as a collection of topics, each containing up to 10 posts at a time. 

The server uses a hash table to store the topics: this way, users can be served in close to O(1) time (because I use an extremely efficient hash function called djb2, and the ratio of the number of items stored to the number of spaces in my table is very small, there are usually 0 collisions. In the case of collisions, however, a linked list is used to chain elements at that index of the table.

After the server program is started, the client can connect using the ip address of the machine running the server and the port which the server is using. From there, clients can post topics, post messages within topics, view the messages in topics, and view a list of the topics that have been created.

When the server process is terminated, the server will write its current state to a binary file. Later, when the server is restarted, if the binary file is in that directory, the server will reconstruct the hash table using the binary file.

The primary features of this library are in the hash table, including basic insertion and deletion operations, as well as the more complex binary file operations. They are documented in the FunctionDocumentation.md file. The rest of the functions I use (including the various socket wrappers and server/client functions) are documented in their header and implementation files.

# Dependencies
* This program uses the standard C and C++ libraries, and it is necessary to have access to GCC for compilation.
* Serialization of transmitted bytes has not been implemented, so it is important that systems using the program are running Ubuntu 14.04 and using GCC 4.8.2. However, I have tried connecting from my Mac (running OS X Yosemite 10.10.2 and not using gcc but rather Apple's LLVM version 6.1.0 and clang-602.0.49) and it works just as well as when I connected using the virtual machine.
* This code was written for Unix systems, including Mac OS X and Ubuntu Linux.

# System Requirements
* C/C++ compiler is required to compile the source code.
* Unix systems should be able to run the server and client programs.
* Windows users will not be able to use this library, Solaris users may have issues as well.

# Group Members
Peter Delevoryas

# Contributors

# Open Issues/Bugs
* Issue: very little error checking between processes
* Issue: if two servers are running on the same port, client does not indicate there is an issue
* Bug: If a user overflows the message posting buffer, it appears to reprint the username in the next post.
* Issue: Serialization: I'm doing any! I'm sending structs over the socket as byte arrays, so the entire project
    requires that connected machines have the same representation of bytes and the same compiler (cause struct padding)

# More Information About the Transmission Process
This simple Server/Client library implements communication between processes, even on different computers, however there is one catch! My program uses the fact that we can transfer bytes across the socket using the read() and write() functions, however unfortunately this will only work on the same computer or very similar computers, as byte representations and compilers will result in different program interpretations, and the data will not be transferred in a standard way. I plan to serialize the data in this program as soon as I can, but since the VM's are all the same, it doesn't really matter, as they all have the same representations of the program. However, it will be impossible to connect with other users using the VM, because the VM uses a NAT to shield itself from the outside world, and without more sophisticated programming methods, there is no way for me to facilitate communication between different VMs. So take my word for it, this program does work across multiple computers! I've tried it with two macs running OS X natively, and the connection was made and the data transferred was correct. Unfortunately, you will likely only be able to simulate this using multiple terminal windows on your single computer.
