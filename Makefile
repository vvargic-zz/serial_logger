default: serial_logger test_client

serial_logger.o: serial_logger.c
	gcc -c serial_logger.c -o serial_logger.o

test_client.o: test_client.c
	gcc -c test_client.c -o test_client.o

serial_logger: serial_logger.o
	gcc serial_logger.o -o serial_logger

test_client: test_client.o
	gcc test_client.o -o test_client

clean:
	-rm -f serial_logger.o serial_logger test_client.o test_client
