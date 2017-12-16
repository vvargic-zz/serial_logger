# serial_logger

	Application for logging raw data from multiple serial ports

	serial_logger will block on waiting for data to be ready
	on any of the available ports. It will read all bytes in
	between STX (0x02) and ETX (0x03) special characters.

	In order to test its functionality, test_client application
	is given to be used together with "socet" app.

	In-depth description and instructions are given in README.txt

