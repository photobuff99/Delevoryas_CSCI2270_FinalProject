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
    6. Also try entering help at the client prompt for more info on commands.
    7. If help and exit aren't working, there may be a problem with the makefile,
    try compiling the files separately.

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
    I haven't done very much testing with it, but otherwise there aren't any
    major bugs or issues that I am aware of.

# More Information
This simple Server/Client library implements communication between processes, even on different computers, however there is one catch! My program uses the fact that we can transfer bytes across the socket using the read() and write() functions, however unfortunately this will only work on the same computer or very similar computers, as byte representations and compilers will result in different program interpretations, and the data will not be transferred in a standard way. I plan to serialize the data in this program as soon as I can, but since the VM's are all the same, it doesn't really matter, as they all have the same representations of the program. However, it will be impossible to connect with other users using the VM, because the VM uses a NAT to shield itself from the outside world, and without more sophisticated programming methods, there is no way for me to facilitate communication between different VMs. So take my word for it, this program does work across multiple computers! I've tried it with two macs running OS X natively, and the connection was made and the data transferred was correct. Unfortunately, you will likely only be able to simulate this using multiple terminal windows on your single computer.
