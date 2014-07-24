/*
 * qusb_helpers.h
 *
 *  Created on: Feb 15, 2011
 *      Author: wibble
 */

#ifndef EPP_HELPERS_H_
#define EPP_HELPERS_H_

#include <stdbool.h>
#include <stdint.h>

#include "qusb_struct.h"
#include "simple_fifo.h"
#include "defaults.h"

int qusb_parse_ports(char *, struct module_list *, struct qusb_opt *);

int qusb_commit(struct simple_fifo *, long int, long int, FILE *);
int int_cmp(const void *, const void *);
void open_cap(int);
void strfifo(char *, short *, int);
void init_opt(struct qusb_opt *);
int parse_opt(struct qusb_opt *, int, char **);
void qusb_log(char *, ...);
void printe(char *, ...);

#endif /* EPP_HELPERS_H_ */
