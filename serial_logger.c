/* Serial logger application
 * Reading input on multiple ports and logging received data
 *
 * date: 12/2017
 *
 * author: Vjekoslav Vargic
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/types.h>

const char *log_file = "serial.log";

//const char *ttyS1 = "/dev/ttyS21";
//const char *ttyS2 = "/dev/ttyS22";
const char *ttyS1 = "/dev/pts/16";
const char *ttyS2 = "/dev/pts/29";

/* Number of seconds to wait on input
	i < 0 => indefinitely
	i = 0 => no waiting
	i = <n> => wait <n> seconds */
const int tmo = -1;

const uint8_t STX = 0x02;
const uint8_t ETX = 0x03;


static int log_char( int fd, uint8_t ch)
{
	if (write(fd, &ch, sizeof(ch)) != 1) {
		printf("\tSomething went wrong with logging.\n");
	} else {
		printf("\tWrote hex byte 0x%2x to log.\n", ch);
	}

	return 0;
}


static int log_string(int fd, const uint8_t *str, int count)
{
	uint8_t i = 0;

	for (i = 0; i < count; i++) {
		log_char(fd, *(str+i));
	}

	return 0;
}


static int read_port(int fd, int fd_log, const uint8_t *str_prefix, size_t ssize)
{
	size_t bytes_read;
	uint8_t buff = 0;
	int stx_flag = 0;

	while ((bytes_read = read(fd, &buff, sizeof(buff))) == 1) {
		if (buff == STX) {
			printf("\treceived STX on fd %d\n", fd);
			log_string(fd_log, str_prefix, ssize);
			stx_flag = 1;
		} else if (buff == ETX) {
			printf("\treceived ETX on fd %d\n", fd);
			log_char(fd_log, '\n');
			break;
		} else if (stx_flag) {
			log_char(fd_log, buff);
		}
	}

	return 0;
}


static int wait_on_input(int fd1, int fd2, int fd_log)
{
	fd_set rfds;
	int max_fd = 1;
	struct timeval *tv;
	int ret;

	FD_ZERO(&rfds);
	FD_SET(fd1, &rfds);
	FD_SET(fd2, &rfds);

	if (tmo >= 0) {	
		tv->tv_sec = tmo;
		tv->tv_usec = 0;
	} else {
		tv = NULL;
	}

	max_fd += (fd1 > fd2) ? fd1 : fd2;
	
	while(1) {
		ret = select(max_fd, &rfds, NULL, NULL, tv);

		if (ret == -1) {
			printf("ERROR: select().\n");
			return 1;
		}

		if (ret) {
			if (FD_ISSET(fd1, &rfds)) {
				printf("\nGot data on fd %d.\n", fd1);
				read_port(fd1, fd_log, "S1: ", 4);
			}
			if (FD_ISSET(fd2, &rfds)) {
				printf("\nGot data on fd %d.\n", fd2);
				read_port(fd2, fd_log, "S2: ", 4);
			}
		} else {
			printf("\nNo data within %d seconds, exiting.\n", tmo);
			break;
		}
	}

	return 0;
}


static int set_attributes(int fd, speed_t bdr)
{
	struct termios attr;

	tcgetattr(fd, &attr);

	cfsetispeed(&attr, B9600);
	cfsetospeed(&attr, B9600);

	// check README note
	cfmakeraw(&attr);

	// one stop bit, no hardware control on input
	attr.c_cflag &= ~(CSTOPB | IXOFF | IXANY);
	if (tcsetattr(fd, TCSANOW, &attr) != 0) {
		printf("\tERROR setting atributes, errno %d.\n", errno);
	}

	// flush stale data
	tcflush(fd, TCIFLUSH);
	printf("Parameters set for fd %d, old data flushed.\n", fd);

	return 0;
}


static int open_port(const char *port_name)
{
	int fd;

	fd = open(port_name, O_RDONLY | O_NOCTTY | O_NDELAY);
	if (fd == -1)
		return -1;

	return fd;
}


static int init_log(const char *log_file)
{
	int fd;

	if (access(log_file, F_OK) != -1) {
		printf("Log file %s already existis, deleting it.\n", log_file);
		if(remove(log_file)) {
			printf("Failed to remove log \"%s\".\n", log_file);
		}
	}

	fd = open(log_file, O_CREAT | O_WRONLY);
	if (fd == -1) {
		return -1;
	}

	return fd;
}


/* Entrance to program */
int main(int argc, char **argv)
{
	int fd1, fd2, fd_log;
	const char *log_file = "console.log";
	
	printf("\nStarting serial logger.\n\n");

	fd1 = open_port(ttyS1);
	if (fd1 == -1) {
		printf("Open %s failed.\n", ttyS1);
		return -1;
	}
	printf("Sucessfully opened port %s, fd %d.\n", ttyS1, fd1);

	fd2 = open_port(ttyS2);
	if (fd2 == -1) {
		printf("Open %s failed.\n", ttyS2);
		return -1;
	}
	printf("Sucessfully opened port %s, fd %d.\n", ttyS2, fd2);

	fd_log = init_log(log_file);
	if (fd_log == -1) {
		printf("Open %s failed.\n", log_file);
		return -1;
	}
	printf("Sucessfully opened new log file \"%s\", fd %d.\n", log_file, fd_log);
	
	set_attributes(fd1, B9600);
	set_attributes(fd2, B9600);

	printf("\nWaiting on data...\n");
	wait_on_input(fd1, fd2, fd_log);

	close(fd1);
	close(fd2);
	close(fd_log);

	printf("\nClosing serial logger.\n\n");
	return 0;
}


