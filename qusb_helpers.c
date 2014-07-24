/*
 * qusb_helpers.c
 *
 *  Created on: Feb 15, 2011
 *      Author: wibble
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <stdarg.h>
#include <time.h>

#include "simple_fifo.h"
#include "qusb_errors.h"
#include "qusb_helpers.h"
#include "defaults.h"

int qusb_parse_ports(char *port_string, struct module_list *portl, struct qusb_opt *o) {
	/*
	 * Parses a port name string returned by QuickUsbFindModules(),
	 * putting individual names into the portl->portv string array,
	 * and returning the number of detected QuickUSB modules.
	 */

	int i = -1;
	char *base;

	base = &port_string[0];
	strcpy(portl->portv[0], base);

	i = 0;
	while (portl->portv[i][0] != 0) {
		base = memchr(base, 0, 10000) + 1;
		i++;

		strcpy(portl->portv[i], base);
	}

	if (o->verbose) {
		printf("Number of names: %i.  [", i);
		for (int j = 0; j < i; j++) {
			printf(" %s", portl->portv[j]);
		}
		printf(" ]\n");
	}

	portl->portc = i;

	return(i);
}

void printe(char *format, ...) {
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	fflush(stderr);
	va_end(args);
}

void qusb_log(char *format, ...) {
	static FILE *log = NULL;
    static bool init = true;
	static time_t start_time;
	char ostr[1024];
	struct tm ct;
	va_list args, iarg;

    va_start(args, format);

    if (init) {
    	// First run.  Initialize logs.

		va_copy(iarg, args);

		printf("Logging to");fflush(stdout);

		openlog("qusb_acq", LOG_CONS|LOG_NDELAY, LOG_LOCAL7);
		printf(" syslog");fflush(stdout);

		start_time = va_arg(iarg, time_t);
		gmtime_r(&start_time, &ct);
		sprintf(ostr, "%s-%04i%02i%02i-%02i%02i%02i-epp.log", format,
				ct.tm_year+1900, ct.tm_mon+1, ct.tm_mday, ct.tm_hour, ct.tm_min, ct.tm_sec);
		log = fopen(ostr, "a+");
		if (log == NULL) {
			syslog(LOG_ERR, "[T+%li] Opening log file '%s' failed!", time(NULL)-start_time, ostr);
			fprintf(stderr, "Opening log file '%s' failed!\n", ostr);
		} else {
			printf(" and %s", ostr); fflush(stdout);
			fprintf(log, "Logging to syslog and %s started.\n", ostr); fflush(log);
		}
		printf(" started.\n"); fflush(stdout);

		init = false;
    }
    else {
		vsprintf(ostr, format, args);
		syslog(LOG_ERR, "[T+%li] %s", time(NULL)-start_time, ostr);

		if (log != NULL) fprintf(log, "[qusb_acq T+%li] %s\n", time(NULL)-start_time, ostr);fflush(log);
    }
	va_end(args);
}

int parse_opt(struct qusb_opt *options, int argc, char **argv) {
	int ports[MAXPORTS];
	char *pn;
    int c, i = 0;

    while (-1 != (c = getopt(argc, argv, "A:x:p:o:OP:S:C:R:m:rd:a:vVh"))) {
		switch (c) {
			case 'A':
				options->acqsize = strtoul(optarg, NULL, 0);
				break;
            case 'x':
				options->maxacq = strtoul(optarg, NULL, 0);
				break;
            case 'p':
				pn = strtok(optarg, ",");
				ports[i] = strtoul(pn, NULL, 0);
				while ((pn = strtok(NULL , ",")) != NULL) {
					i++;
					ports[i] = strtoul(pn, NULL, 0);
				}
				qsort(ports, i+1, sizeof(int), int_cmp); // Sort port list
				for (int j = 0; j <= i; j++) options->ports[j] = ports[j];
				options->mport = i+1;
				break;
            case 'P':
            	options->prefix = optarg;
            	break;
            case 'o':
            	options->outdir = optarg;
            	break;

            case 'S':
				options->fifosize = strtoul(optarg, NULL, 0);
				break;
            case 'R':
				options->rtdsize = strtoul(optarg, NULL, 0);
				break;
            case 'm':
				options->rtdfile = optarg;
				break;
            case 'd':
				options->dt = strtod(optarg, NULL);
				break;
            case 'a':
				options->rtdavg = strtoul(optarg, NULL, 0);
				break;

            case 'v':
				options->verbose = true;
				break;
            case 'V':
				options->debug = true;
				break;
            case 'h':
            default:
				printf("qusb_acq: Acquire data from IEEE1284 parallel port(s).\n\n Options:\n");
				printf("\t-A <#>\tAcquisition request size [Default: %i].\n", DEF_ACQSIZE);
				printf("\t-x <#>\tMax <acqsize>-byte acquisitions [Inf].\n");
				printf("\t-p <#>\tPort(s) to acquire from (see below) [1].\n");
				printf("\t\tCan either give a single port, or a comma-separated list.\n");
				printf("\t-P <s>\tSet output filename prefix [%s].\n", DEF_PREFIX);
				printf("\t-o <s>\tSet output directory [%s].\n", DEF_OUTDIR);
				printf("\n");
				printf("\t-R <#>\tReal-time display output size (in words) [%i].\n", DEF_RTDSIZE);
				printf("\t-m <s>\tReal-time display file [%s].\n", DEF_RTDFILE);
				printf("\t-d <#>\tReal-time display output period [%i].\n", DEF_RTD_DT);
				printf("\t-a <#>\tNumber of RTD blocks to average [%i].\n", DEF_RTDAVG);
				printf("\n");
				printf("\t-v Be verbose.\n");
				printf("\t-V Print debug-level messages.\n");
				printf("\t-h Display this message.\n");
				exit(1);
		}

    }

    return argc;
}

/* qsort int comparison function */
int int_cmp(const void *a, const void *b)
{
    const int *ia = (const int *)a; // casting pointer types
    const int *ib = (const int *)b;
    return *ia  - *ib;
	/* integer comparison: returns negative if b > a
	and positive if a > b */
}

void init_opt(struct qusb_opt *o) {
	memset(o, 0, sizeof(struct qusb_opt));
	o->acqsize = DEF_ACQSIZE;
	memset(o->ports, 0, sizeof(int) * MAXPORTS); o->ports[0] = 1;
	o->mport = 1;
	o->oldsport = false;
	o->prefix = DEF_PREFIX;
	o->outdir = DEF_OUTDIR;

	o->rtdsize = DEF_RTDSIZE;
	o->rtdfile = DEF_RTDFILE;
	o->dt = DEF_RTD_DT;
	o->rtdavg = DEF_RTDAVG;

	o->debug = false;
	o->verbose = false;
}
