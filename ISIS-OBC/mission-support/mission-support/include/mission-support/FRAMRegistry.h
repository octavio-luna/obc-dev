/*
 * FRAMRegistry.h
 *
 *  Created on: 18 okt. 2018
 *      Author: ASAU
 */

#ifndef FRAMREGISTRY_H_
#define FRAMREGISTRY_H_

#include <hal/boolean.h>

/*!
 * Verifies if some FRAM space is be available
 *
 * @param FRAMStart Start address to use in search
 * @param FRAMEnd End address to use in search
 * @return Returns TRUE if someone else claims to be using and address within start/end given
 */
Boolean FRAMReg_IsOccupied(unsigned int FRAMStart, unsigned int FRAMEnd);

/*!
 * Registers an address in FRAM space as occupied
 *
 * @param FRAMStart Start address of FRAM to register as occupied
 * @param FRAMEnd End address of FRAM to register as occupied
 * @param sourceStr pointer to the start of a null terminated string for user of the FRAM memory
 * @param id a unique integer which identifies the FRAM user uniquely
 * @return Returns TRUE if address space was successfully registered as occupied
 */
Boolean FRAMReg_SetOccupied(unsigned int FRAMStart, unsigned int FRAMEnd, unsigned char* sourceStr, int id);

/*!
 * Prints current registered occupied FRAM space
 */
void FRAMReg_PrintMapping(void);

/*
 * CAUTION: This function should be used with utmost caution
 * This will attempt to remove a single entry
 * Returns FALSE if the call failed
 */
Boolean FRAMReg_SetAvailable(unsigned int FRAMStart, unsigned int FRAMEnd);

/*
 * CAUTION: This function is ONLY provided for testing purposes
 * This will wipe the entire registry
 */
void FRAMReg_Empty(void);

#endif /* FRAMREGISTRY_H_ */
