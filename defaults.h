/*
 * defaults.h
 *
 *  Created on: Mar 9, 2011
 *      Author: wibble
 */

#ifndef DEFAULTS_H_
#define DEFAULTS_H_

#define DEF_ACQSIZE 131584  // Data acquisition request size
                            // (65536 + 22 head + 4 foot) words
#define DEF_PREFIX "rxdsp"
#define DEF_OUTDIR "/daq"

#define DEF_CFSIZE 65536 // Colonel Frame size, words
// #define DEF_CFHEAD "Dartmouth College "
#define DEF_CFHEAD "aDtromtu hoCllge e"

#define DEF_RTDSIZE 65536 // RTD Output size, words
#define DEF_RTDFILE "/tmp/rtd/rtd.data"
#define DEF_RTD_DT 0 // No RTD by default
#define DEF_RTDAVG 12

#define MAXPORTS 16

#endif /* DEFAULTS_H_ */
