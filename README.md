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
the same version of GCC, so the data
should transfer correctly!
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
* Serialization of transmitted bytes has not been implemented, so it is important that systems using the program are using GCC 4.8.2 (struct padding). However, I have tried connecting from my Mac (running OS X Yosemite 10.10.2 and not using gcc but rather Apple's LLVM version 6.1.0 and clang-602.0.49) and it works just as well as when I connected using the virtual machine.
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
* Issue: Killing the nohup process seems to be preventing the hash table destructor (which writes the binary file) from being called, despite the use of a signal handler which explicitly calls the destructor before exiting.

# More Information About the Transmission Process
This program does not using serialization because the data I'm transmitting is in the form of character arrays: even though I'm using structs, since all the fields are character arrays, it works out fine. This is of course because the read() and write() functions automatically take the the information being transmitted and convert it to network byte order, then convert it back to the native byte order on the other side. Using a different compiler might affect the struct padding and thus the sender might send a number of bytes that is smaller/larger than the expected size, and the receiver would be expecting a number of bytes that is larger/smaller than what it would receive, thus producing an error message. I have only tested with Apple's LLVM version 6.1.0, and GCC 4.8.2, but they both have the same size of struct for Title and Post and Request, so they should work together. I have not examined the size of the structs on Windows. To see if the program will work on your machine, simply 
            cout << sizeof(Post) << ' ' << sizeof(Test) << endl;
The result you should get, for compatability, is "2102 21121"
