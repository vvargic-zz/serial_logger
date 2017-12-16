# serial_logger

Application for logging raw data from multiple serial ports


1. Compiling

- application is developed on Linux Ubuntu 14.04 LTS, x64

- compiling is done with GNU Compiler Collection, version:
	gcc version 4.8.4 (Ubuntu 4.8.4-2ubuntu1~14.04.3) 

- for compiling, Makefile is given and should be use with command:
	make

- for removing executables and temporary files, use:
	make clean


2. Executables

- make will create two executables:
	serial_logger and test_client

- serial logger will try to open two serial ports given by constants in
  source code:
	const char *ttyS1 = "/dev/ttyS21";
	const char *ttyS2 = "/dev/ttyS22";

- in order to use different ports, these constants need to be set appropriately
  and application needs to be recompiled

- test client opens two serial ports:
	const char *ttyS1 = "/dev/pts/8";
	const char *ttyS2 = "/dev/pts/28";
  and is intended to be used with "socat" application to test serial_logger


3. Testing application

- for testing purposes test_client is developed and "socat" application
  needs to be downloaded:
	sudo apt-get install socat

- "socat" will create virtual ports which then can be used to emulate serial device.
  In order to create virtual port, following command should be executed in
  separate terminal:
	socat -d -d pty,raw,echo=0 pty,raw,echo=0

  This will create two ports, one for input of data and one for output of data.

  Example output of running command is:
	$ socat -d -d pty,raw,echo=0 pty,raw,echo=0
	2017/12/16 11:04:18 socat[15884] N PTY is /dev/pts/28
	2017/12/16 11:04:18 socat[15884] N PTY is /dev/pts/29
	2017/12/16 11:04:18 socat[15884] N starting data transfer loop with FDs [3,3] and [5,5]

	Here, port "/dev/pts/28" is used for writing data and port
	"/dev/pts/29" is used for reading data.

- Command can be run multiple times in separate terminals in order to create
  multiple virtual ports which can be then used with test app

- After 4 virtual ports are created (2 input, 2 output), user needs to manually
  change used ports in serial_logger.c and test_client.c where serial_logger.c
  will be given two ports for reading and client_logger.c will be given two
  ports for writing.

- Afterwards application can be started in separate terminals as:
	./serial_logger
	./test_client

- Application will give output in order for user to see what is going on;
  data read with serial_logger will stay in file "console.log" afterwards, where
  input from first port starts in newline with "S1: " and input from second
  port starts in newline with "S2: "

- Data buffer for writing is hardcoded and can be changed in test_client.c.
  Buffer needs to start with STX (0x02) and end with ETX (0x03)


4. NOTE

 - Function cfmakeraw() is used to set port attributes. Following is excerpt
   from Linux manual page:

	 * cfmakeraw() sets the terminal to something like the "raw" mode of the old
	 * Version 7 terminal driver: input is available character by character,
	 * echoing is disabled, and all special processing of terminal input and
	 * output characters is disabled. The terminal attributes are set as follows:

	termios_p->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
			| INLCR | IGNCR | ICRNL | IXON);
	termios_p->c_oflag &= ~OPOST;
	termios_p->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	termios_p->c_cflag &= ~(CSIZE | PARENB);
	termios_p->c_cflag |= CS8;

cfmakeraw():
   Since glibc 2.19:
       _DEFAULT_SOURCE
   Glibc 2.19 and earlier:
       _BSD_SOURCE

 - Function tcsetattr() will return true even if all attributes were not set.
   Following is excerpt from Linux manual page:

	 * Note that tcsetattr() returns success if any of the requested changes could be
	 * successfully carried out. Therefore, when making multiple changes it may be
	 * necessary to follow this call with a further call to tcgetattr() to check that
	 * all changes have been performed successfully.


5. TODO

Serial_logger.c:
 - accept terminal arguments for port1 path, port2 path and log file path
 - add "-h" flag with appropriate help message
 - add support for more than two ports

Test_client.c:
 - accept terminal arguments for port1 path, port2 path and data buffer
   for sending to ports
 - add "-h" flag and appropriate help message


