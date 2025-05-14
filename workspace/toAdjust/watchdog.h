/*
e * watchdog.h
 *
 *  Created on: 29/04/2024
 *      Author: Luis
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#define EPS_WATCHDOG_INTERVAL_SECS 30
#define EPS_WATCHDOG_CHECK_SECS (EPS_WATCHDOG_INTERVAL_SECS/10)

int reset_eps_watchdog(unsigned int when, void* privData);


#endif /* WATCHDOG_H_ */
