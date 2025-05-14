/*!
 * @file ParamsDB.h
 * @brief A database used to store flight parameters.
 * The database uses RAM and FRAM to store the data. FRAM is a non-volatile memory which
 * is fast and reliable against radiation. It can also sustain exceptionally large amount
 * of write cycles.
 *
 * In the FRAM, parameters database is stored in two-slots alternatively.
 * Each slot is protected with CRC16. If data in the newest slot is corrupted, the ParamsDB_start
 * function can attempt to restore it from the other slot. If restoring data from FRAM fails,
 * the parameters can be reset to their default values and stored.
 *
 * The database also stores the same data in RAM to provide a sort of scratch-pad like functionality
 * to the user. The RAM can also be used as a quick access for getting parameters.
 * When a restore succeeds, the parameter-values stored in FRAM are also placed in RAM, otherwise
 * the RAM and FRAM databases are reset to the default parameter values.
 */

#ifndef PARAMSDB_H_
#define PARAMSDB_H_

#include <hal/boolean.h>

#define E_PARAMSDB_NO_ERROR			 0 //!< No error occurred in the parameters database.

#define E_PARAMSDB_FRAM_ERROR		-1 //!< FRAM malfunctioned.
#define E_PARAMSDB_RAM_ERROR		-2 //!< RAM malfunctioned.
#define E_PARAMSDB_MALLOC_ERROR		-3 //!< Memory allocation failed.
#define E_PARAMSDB_INVALID_INPUT	-4 //!< One of the inputs to the function is invalid.
#define E_PARAMSDB_PARAM_NOT_FOUND	-5 //!< Parameter was not found, likely index out-of-range.
#define E_PARAMSDB_RESTORE_FAILED	-6 //!< ParamsDB_start attempted a restore from FRAM and failed.
#define E_PARAMSDB_FRAM_ADDR		-7 //!< Attempting to use an invalid FRAM address
#define E_PARAMSDB_FRAM_OVERLAP		-8 //!< The desired FRAM space has been registered for some other use
#define E_PARAMSDB_INCORRECT_SIZE 	-9 //!< The provided size does not match the configured size
#define E_PARAMSDB_ALREADY_STARTED	-10 //!< The ParamsDB has already been started and not be started again

#define E_PARAMSDB_RESET_SUCCESS	 1 //!< ParamsDB_start had to reset the database to defaults.

/*!
 * Structure used to store a parameter.
 * @note When passing an instance of this structure to the functions,
 * make sure data points to adequate memory space allocated to store the parameter's data.
 */
typedef struct  __attribute__ ((__packed__)) _Param {
	unsigned int size; //!< Size of the parameter's data in bytes.
	void *data; //!< Pointer to a location where the parameter's data should be stored.
} Param;

/*!
 * Configuration used to initialize the parameters database.
 * @note The space taken up by the parameters database in FRAM is:
 * (numberOfParams*4 + Sum(ParameterSizes) + 6) * 2
 */
typedef struct _ParamsDBConfig {
	unsigned int numberOfParams; //!< Total number of parameters to stored in the database.
	unsigned int FRAMaddress; //!< Starting address where parameters will be stored in FRAM.
	Param *defaultParamValues; //!< Pointer to an array of Param structures containing default values for all parameters.
} ParamsDBconfig;

/*!
 * Indicates what kind of recovery options parameter database can use while starting up.
 */
typedef enum _ParamsResetLevel {
	paramsDB_noReset = 0,      //!< paramsDB_noReset This prohibits the database from resetting all parameters to defaults if FRAM data is corrupted beyond recovery.
	paramsDB_resetIfNeeded = 1,//!< paramsDB_resetIfNeeded This allows the database to all parameters to defaults only if FRAM data is corrupted beyond recovery.
	paramsDB_forceReset = 2    //!< paramsDB_forceReset This forces the database to reset all parameters to their default values.
} ParamsResetLevel;

/*!
 * Indicates the source or destination storage media used by ParamsDB_getParam or ParamsDB_setParam
 */
typedef enum _ParamsStorage {
	paramsDB_RAM = 0, //!< paramsDB_RAM
	paramsDB_FRAM = 1,//!< paramsDB_FRAM
	paramsDB_all = 2  //!< paramsDB_all This can only be used with ParamsDB_setParam.
} ParamsStorage;

/*!
 * Initializes the parameters database.
 * @param config Configuration used to initialize the parameters database.
 * @param resetLevel Indicates what kind of recovery options can be used by the database.
 * @return E_PARAMSDB_NO_ERROR on success, E_PARAMSDB_RESET_SUCCESS when a reset was performed successfully,
 * negative values for errors.
 */
int ParamsDB_start(ParamsDBconfig *config, ParamsResetLevel resetLevel);

/*!
 * De-Initializes the parameters database.
 * @return returns E_PARAMSDB_NO_ERROR on success, E_PARAMSDB_FRAM_ERROR.
 * If there is an error the ParamDB will not be stopped as the FRAM will not be available to restart it
 */
int ParamsDB_stop(void);

/*!
 * Returns the status of the FRAM.
 * @return True if FRAM is working as expected FALSE if not.
 */
Boolean ParamsDB_isFRAMworking(void);

/*!
 * Retrieves a parameter and stores it in the given memory location.
 * @param param Location where the parameter is to be stored.
 * @param index Index of the parameter in the defaultParamValues array. This index is used as a unique identifier for parameters.
 * @param source Where the parameter should be retrieved from: Can be paramsDB_RAM or paramsDB_FRAM.
 * @return E_PARAMSDB_NO_ERROR on success, negative values on error.
 */
int ParamsDB_getParam(Param *param, unsigned int index, ParamsStorage source);

/*!
 * Stores the given parameter in the specified destination.
 * @param param Location of a parameter to be stored in the database.
 * @param index Index of the parameter in the defaultParamValues array. This index is used as a unique identifier for parameters.
 * @param destination Where the parameter should be stored: Can be paramsDB_RAM or paramsDB_FRAM or paramsDB_all.
 * @return E_PARAMSDB_NO_ERROR on success, negative values on error.
 */
int ParamsDB_setParam(Param *param, unsigned int index, ParamsStorage destination);

#endif /* PARAMSDB_H_ */
