# Tiny Operating System
A basic operating system developed as term project for Advanced Operating System class.
It is based on x86 IBM PC and supports pre-emptive multitasking using interrupt handling. The OS features segmentation to implement memory separation and have a protocol 
for Interprocess Communication as well. The display is operated in text mode and uses memory mapped IO to render characters on the screen. 
It involved assembly language programming as well to achieve some magic tricks that are impossible for any high level programming language.
To make it interactive, I implemented a shell process with some basic commands (type help), a Pong game process and a model 
train controller process that controls a train on a model track via serial line communication channel on RS232.


