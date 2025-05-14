/*!
 * @file Log.h
 *
 * @brief A logging library that can be used to sequentially store any kind of data such as:
 * error logs, housekeeping data or payload data. There is a maximum number of logs that
 * can exist simultaneously, as defined by LOG_MAX_NO_OF_LOGS
 *
 * This module is designed to store data redundantly in multiple locations including
 * RAM, FRAM and SD-Card file. The number of log-entries to store in each media can be configured
 * on startup. This ensures that there is at least some data available in the small but very reliable
 * non-volatile storage of FRAM while using SD-Card as the mass-media. RAM can be used for quick-retrieval
 * of the past-data.
 *
 * The latest entries are stored redundantly in all-enabled storage media.
 * To demonstrate, suppose maxEntriesInFRAM = 4, maxEntriesInRAM = 10 and maxEntriesInFile = 512
 * This means, the newest 4 entries will be present in all-3 storage media, the next 6 entries
 * will be present in both RAM and SD-Card and older entries will be only present in a file on the SD-Card.
 *
 * When the user requests to read an entry from the Log for the first time, the logger first searches
 * the RAM, then FRAM, and lastly the file. In this way, faster memory is accessed first for retrieving data.
 * When the user requests further entries, first the entry that last returned data is tried.
 * If data is not found there, other memories are searched.
 *
 * The log data is protected by adding an 8-bit CRC to every entry in the log.
 * The module also adds the current timestamp whenever any data is added to the log.
 */

#ifndef LOG_H_
#define LOG_H_

#include <hal/boolean.h>
#include <hal/Timing/Time.h>
#include <hcc/api_fat.h>

// The error values are powers of 2 so that cumulative errors can be returned easily
// So an error value of -6 means both E_LOG_FS_ERROR and E_LOG_FRAM_ERROR errors occurred.
#define E_LOG_NO_ERROR			 0   //!< No error occurred.
#define E_LOG_FRAM_CLEARED		-1	 //!< FRAM was cleared on start-up because it was too far out of sync with File
#define E_LOG_FS_ERROR			-2   //!< File-System error (SD-Card).
#define E_LOG_FRAM_ERROR		-4   //!< Error while reading or writing FRAM.
#define E_LOG_RAM_ERROR			-8   //!< Error while manipulating log-data in RAM.
#define E_LOG_MALLOC_ERROR		-16  //!< Error while allocating memory.
#define E_LOG_INVALID_INPUT		-32  //!< User-input was invalid.
#define E_LOG_TIMESTAMP_ERROR	-64  //!< Error while retrieving time to be added to a log-entry. The log will still store data if this error happens.
#define E_LOG_ENTRY_NOT_FOUND	-128 //!< Unable to find the requested log-entry.
#define E_LOG_FRAM_ADDR 		-256  //!< Attempting to use an invalid FRAM address
#define E_LOG_FRAM_OVERLAP 		-512  //!< Attempting to use FRAM space allocated for something else
#define E_LOG_MAX_LOGS 			-1024 //!< Exceeding max number of log

#define LOG_MAX_NO_OF_LOGS		25	//<! The maximum number of simultaneous logs that can be started

/*!
 * A log entry as returned by functions that retrieve data from the logs.
 */
typedef struct __attribute__ ((__packed__)) _LogEntry { // Do not reorder this struct
	unsigned char crc8; //!< 8-bit CRC.
	unsigned long time; //!< Time at which the log-entry was stored.
	unsigned int entryNumber; //!< A unique-identifier for the log-entry that is incremented each time an entry is succesfully added to a log.
	unsigned char data[]; //!< The data for the log-entry. Note that this is a variable size member.
} LogEntry;

/*!
 * A LogInit structure contains all the information required to start / initialize a log
 *
 * @note If a log is initialized with maxEntriesInFile set to 0, no data for that log will be stored in the SD-Card.
 * Similarly, if maxEntriesInFRAM is set to 0, no data for that log will be stored in FRAM.
 */
typedef struct _LogInit
{
	unsigned int entryDataSize; //!< Number of bytes of data in a log entry. Size of a stored entry is sizeof(LogEntry) + entryDataSize.
	unsigned int maxEntriesInFRAM; //!< Maximum number of entries that should be stored in FRAM. Can be 0. Setting this to 0 will cause no log to be maintained in FRAM.
	unsigned int maxEntriesInRAM; //!< Maximum number of entries that should be stored in RAM. Minimum 1.
	unsigned int maxEntriesInFile; //!< Maximum number of entries that should be stored in the log file on SD-Card. Can be 0. Setting this to 0 will cause no log to be maintained on SD-Card.
	char logFilePath[256]; //!< Path of the log file. Must be unique. Not required if maxEntriesInFile is 0.
	Boolean fileWriteVerify; //!< Verify whether log writes to SD cards were successful (can be used as workaround for SD card caching issues)
	Boolean fileFlushOnAdd; //!< Indicate whether to flush / commit data to the disk when adding an entry
	unsigned int FRAMlogBaseAddress; //!< Address in FRAM where the first entry of the log will be stored. Not required if maxEntriesInFRAM is 0.
} LogInit;

/*!
 * A LogStart structure contains statistics about a log
 */
typedef struct _LogStats
{
	unsigned int entriesInFile; //!< Number of valid entries in the file where the log is currently being written.
	unsigned int entriesInRAM; //!< Number of valid entries present in RAM.
	unsigned int entriesInFRAM; //!< Number of valid entries for the log currently present in FRAM.

	unsigned int entrySize; //!< Size of an actual log-entry in bytes = entryDataSize + sizeof(LogEntry).
	unsigned int nextEntryNumber; //!< Number of the next log-entry.

	Boolean RAMstorageWorking; //!< True if RAM log storage is working fine.
	Boolean FRAMstorageWorking; //!< True if FRAM log storage is working fine.
	Boolean FilestorageWorking; //!< True if File log storage is working fine.
} LogStats;

/*!
 * Storage source of a log entry.
 * The library will first try to retrieve the entry from RAM, then from FRAM and then from SD-Card.
 */
typedef enum _LogEntrySource {
	none_log, //!< none_log
	RAM_log, //!< RAM_log
	FRAM_log,//!< FRAM_log
	File_log //!< File_log
} LogEntrySource;

/*!
 * A log reader structure is comparable to a file structure of the file system library.
 * The Log_readStart function will initialize this which then needs to be passed to
 * Log_readNext and Log_readPrevious functions to continue reading entries from the log.
 *
 * @note Members of this structure are for internal use by the logger and should be treated
 * as READ-ONLY by user code.
 */
typedef struct _LogReader {
	int logid; //!< Id of the log this reader is reading.
	unsigned int currentEntryNumber; //!< Entry Number that has just been retrieved.
	LogEntrySource source; //!< Source from which last entry was retrieved.
	F_FILE* logFile; //!< File structure for reading the Log-file if needed.
} LogReader;

/*!
 * Initializes a log. Any number of logs may be initialized simultaneously.
 * @note This is not a re-entrant safe function
 * @param[in] loginit Structure containing log-configuration.
 * @param[in] firstInit If this is the first-time this particular log is created,
 * this function needs to clear some space in FRAM and create a File in SD-Card
 * so this parameter must be set to true for the first time, false otherwise.
 * @param[out] logid ID of the log that was started, -1 if log was not started at all
 * @return negative values on error. See error defines for details.
 *
 * @see Log
 */
int Log_start(LogInit *loginit, Boolean firstInit, int* logid);

/*!
 * Clears up the data stored for the given log.
 * @note The Log file will be truncated so all LogReaders need to be stopped before calling this function, otherwise
 * the file truncation will fail.
 * @param logid Identifier for the log.
 * @return 0 on success, negative values on error. See error defines for details.
 */
int Log_clear(int logid);

/*!
 * Clears up the data stored for the given log in RAM.
 * @param logid Identifier for the log.
 */
void Log_clearRAM(int logid);

/*!
 * Force a flush / commit of the Log file, writing any outstanding log entries to the disk
 * @note Intended for use when fileFlushOnAdd in LogInit is set to FALSE
 * @param logid Identifier of the log.
 * @return 0 on success, negative values on error. See error defines for details.
 */
int Log_flushLogFile(int logid);

/*!
 * Get statistics for a given log
 * @param logid Identifier for the log.
 * @param stats Location where the log's stats should be stored
 * @return 0 on success, -1 on error.
 */
int Log_getStats(int logid, LogStats* stats);

/*!
 * Adds an entry into the log.
 * As long as adding an entry to at least one of the storage media succeeds,
 * the added data is available in the log and nextEntryNumber is incremented.
 * @param logid Identifier for the log.
 * @param data Pointer to the data to be added to the log.
 * @return 0 on success, negative values on error. See error defines for details.
 *
 * @note In case of E_LOG_TIMESTAMP_ERROR, this function will still store the entry
 * but will 0xFF as year and 0xFFFFFFFF as secondsOfYear for the timestamp.
 */
int Log_add(int logid, void *data);

/*!
 * Reads the entry that was most recently added to the log.
 * @param logid Identifier for the log
 * @param entry Location where the read entry should be stored.
 * @return 0 on success, negative values on error. See error defines for details.
 *
 * @note Make sure to allocate the right size to store the retrieved log entry:
 * @code logEntryBuffer = malloc(sizeof(LogEntry) + log.entryDataSize); @endcode
 */
int Log_readLastEntry(int logid, LogEntry *entry);

/*!
 * Reads a single entry from the log.
 * @param logid Identifier for the log
 * @param entryNumber The entry-number that must be retrieved.
 * @param entry  Location where the read entry should be stored.
 * @return 0 on success, negative values on error. See error defines for details.
 *
 * @note Make sure to allocate the right size to store the retrieved log entry:
 * @code logEntryBuffer = malloc(sizeof(LogEntry) + log.entryDataSize); @endcode
 */
int Log_readOnce(int logid, unsigned int entryNumber, LogEntry *entry);

/*!
 * Starts up the process of reading multiple entries from the log.
 * After calling this function, Log_readNext or Log_readPrevious can be used
 * to continue reading entries from the log.
 * @param logid Identifier of the log.
 * @param entryNumber The entry-number of the first entry to be retrieved.
 * @param logReader Location where the logger can store a reader LogReader structure that can be used
 * in the future to read more log-entries.
 * @param entry Location where the logger can store the first entry retrieved.
 * @return 0 on success, negative values on error. See error defines for details.
 *
 * @note Make sure to allocate the right size to store the retrieved log entry:
 * @code logEntryBuffer = malloc(sizeof(LogEntry) + log.entryDataSize); @endcode
 */
int Log_readStart(int logid, unsigned int entryNumber, LogReader *logReader, LogEntry *entry);

/*!
 * Reads the next entry from the log.
 * @note The reading process must be initialized by using Log_readStart before calling this function.
 * @param logReader LogReader used in Log_readStart.
 * @param entry  Location where the logger can store the retrieved entry.
 * @return 0 on success, negative values on error. See error defines for details.
 *
 * @note Make sure to allocate the right size to store the retrieved log entry:
 * @code logEntryBuffer = malloc(sizeof(LogEntry) + log.entryDataSize); @endcode
 */
int Log_readNext(LogReader *logReader, LogEntry *entry);

/*!
 * Reads the previous entry from the log.
 * @note The reading process must be initialized by using Log_readStart before calling this function.
 * @param logReader LogReader used in Log_readStart.
 * @param entry Location where the logger can store the retrieved entry.
 * @return 0 on success, negative values on error. See error defines for details.
 *
 * @note Make sure to allocate the right size to store the retrieved log entry:
 * @code logEntryBuffer = malloc(sizeof(LogEntry) + log.entryDataSize); @endcode
 */
int Log_readPrevious(LogReader *logReader, LogEntry *entry);

/*!
 * Clears the process of reading from a log. This function must be called after user is finished reading
 * from a log with Log_readStart and Log_readNext or Log_readPrevious.
 * @param logReader LogReader used in Log_readStart.
 */
void Log_readStop(LogReader *logReader);

/*!
 * Provides the entry number of the last entry that was added into the log.
 * @param logid Identifier for the log
 * @param entryNumber Location where the logger will store the entry number.
 * @return 0 on success, negative values on error. See error defines for details.
 */
int Log_getLastEntryNumber(int logid, unsigned int *entryNumber);

/*!
 * Verifies whether a LogEntry's crc is still identical to what was stored, i.e. if the LogEntry's data has changed
 * @param logentry Pointer to LogEntry to be verified
 * @param entrysize Size of LogEntry in bytes, this should be Log.stats.entrySize
 * @return TRUE when crc calculated over LogEntry's data is equal to LogEntry's stored crc, i.e. data is unchanged, and FALSE otherwise
 */
Boolean LogEntry_verifyCRC(LogEntry* logentry, unsigned int entrysize);

#endif /* LOG_H_ */
