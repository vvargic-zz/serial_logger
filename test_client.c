/* Serial logger test client
 * Writing data to serial ports
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

const char *ttyS1 = "/dev/pts/8";
const char *ttyS2 = "/dev/pts/28";

#define STX 0x02
#define ETX 0x03

const char send_buff[] = {STX, 0x31, 0x32, 0x33, 0x34, 0x35, ETX};


static int write_to_port(int fd, const char *buff, size_t count) {
	size_t bytes_wrote = 0;

	bytes_wrote = write(fd, buff, count);
	if (bytes_wrote < 0) {
		printf("Write to fd %d failed.\n", fd);
	} else {
		printf("\nSucessfully wrote %ld bytes: %s.\n\n", bytes_wrote, send_buff);
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
		printf("\tERROR setting atributes for fd %d, errno %d.\n", fd, errno);
	}

	// flush stale data
	tcflush(fd, TCIFLUSH);

	return 0;
}


static inline int open_port(const char *port_name)
{
	int fd;

	fd = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1)
		return -1;

	return fd;
}


/* Entrace to program */
int main(int argc, char **argv)
{
	int fd1, fd2;	
	printf("\nStarting serial test.\n\n");

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
	
	set_attributes(fd1, B9600);
	set_attributes(fd2, B9600);

	write_to_port(fd2, send_buff, sizeof(send_buff));
	write_to_port(fd1, send_buff, sizeof(send_buff));

	close(fd1);
	close(fd2);
	
	printf("\nFinished serial test.\n\n");
	return 0;
}


