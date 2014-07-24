#include <stdio.h>
#include <stdlib.h>
#include <QuickUSB.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

#include "qupp.h"

static bool running;

static void do_depart(int signum) {
    running = false;
    fprintf(stderr,"\nStopping...");

    return;
}

int main(){
	QRETURN ret;
	unsigned short int *data;
    QCHAR *port_names;
	QHANDLE dev_handle;
	long read_size, read_bytes, total_read;
	QULONG read_commit;
	struct timeval now, then;
	double time_elapsed;
	QWORD conf_store;
	FILE *ofile;

	struct parport_list portl;

	char fifo_status;
	QULONG ret_err;
	PQULONG ret_err_p = &ret_err;
	bool fifo_acqseq = false, fifo_firsthalf = true;

	signal(SIGINT, do_depart);

	read_size = 32768;
	read_bytes = read_size * sizeof(short int);
    port_names = malloc(100000 * sizeof(char));
    total_read = 0;

	data = malloc(read_bytes);

    ret = QuickUsbFindModules(port_names,100000);
	if (ret == 0) {
		printf("No module found!\n");
		exit(0);
	}

	qusb_parse_ports(port_names, &portl);
	for (int i = 0; i < portl.portc; i++) {
		printf("port %i: %s.\n", i+1, portl.portv[i]);
	}

	ret = QuickUsbOpen(&dev_handle, port_names);
	if (ret == 0) printf("Cannot open device\n");

	printf("Setting...Word-wide...");
	conf_store = 1;
	ret = QuickUsbWriteSetting(dev_handle, 1, conf_store);
	if (ret == 0) printf("fail!!!");

/*	printf("speed...");
	ret = QuickUsbReadDefault(dev_handle, 5, &conf_store);
//	conf_store &= ((1<<15)-1); // Full-speed (12Mbps)
	conf_store |= ((1<<15)); // High-speed (480Mbps)
	ret = QuickUsbWriteSetting(dev_handle, 5, conf_store);
	if (ret == 0) printf("fail!!!");
*/
	printf("IFCLK freq...");
	conf_store = 0b10110010; // Set IFCLK to 30MHz
	ret = QuickUsbWriteSetting(dev_handle, 3, conf_store);
	if (ret == 0) printf("fail!!!");
	printf("done.\n");

	ofile = fopen("junk", "w");
	if (ofile == NULL) {
		printf("Failed to open data file.\n");
		exit(1);
	}

	gettimeofday(&then, NULL);

	now = then;

	running = true;

	printf("reading data!\n");

	while ((running) && (now.tv_sec-then.tv_sec < 60)) {
		read_commit = 1;
		ret = QuickUsbReadPort(dev_handle, 0, (PQBYTE) &fifo_status, (PQWORD) &read_commit);
//		printf("%i %i\n", (fifo_status & 0b10)>>1, (fifo_status & 0b01));
		if (ret == 0) {
			QuickUsbGetLastError(ret_err_p);
			fprintf(stderr, "Failed to read DSP FIFO status: %li.\n", ret_err);
		}

		if (fifo_status & 0b01) {
			// The Acquire Sequence line is high.
			fifo_acqseq = true;

			if (!fifo_firsthalf) {
				fprintf(stderr, "!");
				fifo_firsthalf = true;
			}
		}

		if (!fifo_acqseq) {
			// FIFO not Half-Full or no prior acqseq--do nothing.
			usleep(500);
			continue;
		}

		if  (fifo_status & 0b10) {
			usleep(50);
			continue;
		}

		read_commit = 65536;

		ret = QuickUsbReadData(dev_handle, (PQBYTE) data, (PQULONG) &read_commit);
		if (ret == 0){
			QuickUsbGetLastError(ret_err_p);
			if (ret_err == 6) { // Timeout
//				printf("."); fflush(stdout);
				usleep(50);
				continue;
			} else {
				fprintf(stderr, "Read failed, error code: %li.\n", (long int) ret_err);
				break;
			}
		}
//		printf("read\n"); fflush(stdout);

		if (read_commit != 65536) {
			fprintf(stderr, "Read incomplete? (%lu) ret=%i\n", (unsigned long) read_commit, (int) ret);
		}



		if (fifo_firsthalf) {
			fifo_firsthalf = false;
		}
		else {
			fifo_firsthalf = true;
			fifo_acqseq = false;
		}

		total_read += read_commit;

		ret = fwrite(data, 1, read_commit, ofile);
		if (ret != read_commit) {
			fprintf(stderr, "Failed to write data properly? (%i/%li).\n", (int) ret, read_commit);
		}

		gettimeofday(&now, NULL);
	}

	QuickUsbClose(dev_handle);
	if (ret == 0) {
		printf("Device close failed.\n");
	}

	gettimeofday(&now, NULL);
	time_elapsed = (now.tv_sec-then.tv_sec)+(now.tv_usec-then.tv_usec)/1e6;
	printf("\nRead %li bytes in %f seconds = %f KBps.\n", total_read, time_elapsed, total_read/time_elapsed/1e3);

	return(0);
}

