/*
 * qusb_acq.h
 *
 *  Created on: Feb 23, 2011
 *      Author: wibble
 */

#ifndef EPP_ACQ_H_
#define EPP_ACQ_H_

#define CF_HEADER_SIZE 200 // Colonel Frame header size
#define TIME_OFFSET 946684800 // Time to 2000.1.1 from Epoch

#include "qusb_struct.h"

static void do_depart(int);
void qusb_mport(struct qusb_opt, struct module_list);
void *qusb_data_pt(void *);
void check_acq_seq(QHANDLE, int, bool *);

#endif /* EPP_ACQ_H_ */
