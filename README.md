1) Project Overview
- Contact:
    Author: Leah Arrieta
    E-mail: leaha3@umbc.edu

- Introduction:
    The purpose of this project it to implement a number of functions to create a simple linux shell 
    interface to perform basic linux commands. It offers functionality such as command parsing, execution, 
    reading and displaying data from the /proc filesystem, and allows the user to retrieve their command 
    history to up to 10 lines.

2) Installation and Setup
- Setup
    To develop the environment the Standard C Library and a GCC compiler is needed as the code in in C. 
    This project was made and tested on Linux, so it is reccomended for it to be run on Linux. In addition, 
    there are no environmental variables required. There is a file used for command history, .421sh that is 
    automatically made on startup.

- Build and Compile
    Compiling the project uses the Makefile.
    1. Open the directory will the files: main_template.c, utils.c, utils.h 
    2. Run "make clean"
    3. Run "make run"
    Also, to run valgrind, use "make val".

- Functions
    You can type /proc/cpuinfo to get the cpu information,
    /proc/loadavg for the number of currently running processes
    /proc/process_id_no/status to get various information (detailed in the documentation)
    /proc/process_id_no/environ to list the environment variables
    /proc/process_id_no/sched to isplay performance information
    history to view the last 10 input commands


3) User-Space Driver Programs
- Driver Overview
    This project instead is a user-space shell program with no drivers. However, it interacts with the system 
    components by accessing the /proc filesystem to load information such as: CPU information, load average, uptime, 
    status, environmental variables, and scedule status.

- Building and Running Drivers
    All functionality for project 1 is in the main_template.c and will run following the steps from installation and setup.

4) Testing Strategies
- Test Cases
    1. exit() command:
        purpose: terminate the shell when "exit" is entered by the user
        expected output: the shell terminates

    2. parsing
        purpose: to split user commands into tokens to then be interpreted
        expected output: example, cat /proc/cpuinfo becomes cat /proc /cpuinfo
    
    3. track command history
        purpose: to keep a record of the users last ten entered commands
        expected output: when "histoy" is entered, the last ten commands are displayed
    
    4. memory management
        purpose: ensure no leaks or data is still accessible
        expected output: after running valgrind all data reported as freed

5) Trouble Shooting
- Common issues
    1. "Error: Unable to open..." - occurs from an unknown process or file trying to be accessed
        Solution: Ensure that you are attempting to access an existing file or process and there are 
        no errors in spelling.

    2. "Error: Command not found" - occurs when a command does not exist.
        Solution: Make sure the command exists and is being spelled correctly.

6) References
- External Resources
    1. w3schools - w3schools.com
    2. ReadMe Guide for Student Documentation
    3. Proc Filesystem
    