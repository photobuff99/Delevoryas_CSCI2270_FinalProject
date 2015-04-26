# Project Summary
This library provides code for a simple LAN message board server, along with sample client code for accessing and posting to the message board.
The message board consists of a number of different topics, each containing a series of user-submitted messages.
Users connecting to the server can request to see the current topics, retrieve messages from a topic, or submit a message to a topic.
The server stores the messageboard topics in flat files, while also keeping the last ten messages of each topic in a hashtable.
The server makes use of a hash table to dynamically allocate space for new topics. Topics can be reloaded from
their binary file records, or the binary file "table.bin" that the program creates can be deleted after
turning off the server and the table will begin empty again.

# How to Run
    1. Download the project files to your favorite directory
    2. Use makefile to compile files (type "make")
    3. Run the server program using ./server localhost 9000
    4. In a separate terminal window, run the client program using ./client localhost 9000.
    5. See the documentation for more details on how to use this program for communicating across computers!

# Dependencies
    This program uses the standard C and C++ libraries, and it is necessary to have access to GCC for compilation.
    This code was written for Unix systems, including Mac OS X and Ubuntu Linux.

# System Requirements
    C/C++ compiler is required to compile the source code.
    Unix systems should be able to run the server and client programs.
    Windows users will not be able to use this library, Solaris users may have issues as well.

# Group Members
    Peter Delevoryas

# Contributors

# Open Issues/Bugs
    I haven't done very much testing with it.
