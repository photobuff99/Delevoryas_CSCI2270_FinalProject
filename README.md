# Project Summary
    This library provides code for a simple LAN message board server, along with sample client code for accessing and posting to the message board.
    The message board consists of a number of different topics, each containing a series of user-submitted messages.
    Users connecting to the server can request to see the current topics, retrieve messages from a topic, or submit a message to a topic.
    The server stores the messageboard topics in flat files, while also keeping the last five messages of each topic in a hashtable.
    Topics are updated using a queue, and when requested by the user, can be sorted by number of messages or by most recent post.

# How to Run
    1. Download the project files to your favorite directory
    2. Use makefile to compile files (type "make")
    3. Run server, then use client program to connect to server

# Dependencies
    This program uses the standard C and C++ libraries, and it is necessary to have access to GCC for compilation.
    This code was written for Unix systems.

# System Requirements
    C/C++ compiler is required to compile the source code.
    Unix systems should be able to run the server and client programs.
    Windows users will not be able to use this library, Solaris users may have issues as well.
    
# Group Members
    Peter Delevoryas

# Contributors
    
# Open Issues/Bugs
    ...This project has not been prototyped, much less completed
