#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "qupp.h"

int qusb_parse_ports(char *port_string, struct parport_list *portl) {
	/*
	 * Parses a port name string returned by QuickUsbFindModules(), 
	 * putting individual names into the portl->portv string array,
	 * and returning the number of detected QuickUSB modules.
	 */

	int i = -1;
	char *base;

//	char blahhh[10000] = "QUSB-0\0QUSB-1\0QUSB-2\0QUSB-3\0\0";

	base = &port_string[0];
	strcpy(portl->portv[0], base);

	i = 0;
	while (portl->portv[i][0] != 0) {
		base = memchr(base, 0, 10000) + 1;
		i++;

		strcpy(portl->portv[i], base);
	}

/*	printf("\nNumber of names: %i.  [", i);
	for (int j = 0; j < i; j++) {
		printf(" %s", portl->portv[j]);
	}
	printf(" ]\n");
*/

	portl->portc = i;

	return(i);
}
