#include "LogDemo.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <hal/boolean.h>
#include <mission-support/Log.h>
#include <stdlib.h>

static Boolean logadder_active = FALSE;
static int demolog_id = -1;

/*
 * Example of a structure that represents a log entry. This structure has to be a fixed size!
 * Please note that the structure has been defined as packed so no bytes are wasted on padding, but
 * please beware that this also means that the members of the struct can be unaligned so be careful
 * when using pointers to these members!
 */
typedef struct __attribute__ ((__packed__)) _datablock
{
	char letter;
	unsigned int additional_data[10];
} datablock;

/*!
 * Prints the statistics of a log
 * @param[in] logid ID of the log
 */
static void LogDemo_printLogStats(int logid)
{
	LogStats demolog_stats;

	if(Log_getStats(logid, &demolog_stats) == 0)
	{
		printf("\nStatistics for log id %d\n", logid);
		printf("Entry size: %d bytes - Next entry no.: %d\n", demolog_stats.entrySize, demolog_stats.nextEntryNumber);
		printf("RAM  - ok: %d - %d entries\n", demolog_stats.RAMstorageWorking & 0x01, demolog_stats.entriesInRAM);
		printf("FRAM - ok: %d - %d entries\n", demolog_stats.FRAMstorageWorking & 0x01, demolog_stats.entriesInFRAM);
		printf("SD   - ok: %d - %d entries\n", demolog_stats.FilestorageWorking & 0x01, demolog_stats.entriesInFile);
	}
	else
	{
		printf("\nError occurred while obtaining statistics for log id %d\n", logid);
	}
}

/*!
 * This task adds one entry to the log every 100 milliseconds. The entries will contain a letter and some dummy data.
 * The letter will change from A - Z, then from a - z and then restart at A.
 */
static void LogDemo_taskLogAdder(void* arguments)
{
	datablock exampledata;
	(void)arguments;

	logadder_active = TRUE;

	exampledata.letter = 'A';

	while(logadder_active)
	{
		Log_add(demolog_id, &exampledata);

		if(exampledata.letter == 'Z')
		{
			exampledata.letter = 'a';
		}
		else if(exampledata.letter == 'z')
		{
			exampledata.letter = 'A';
		}
		else
		{
			exampledata.letter++;
		}

		vTaskDelay(100 / portTICK_RATE_MS);
	}

	// Have task delete itself when done
	vTaskDelete(NULL);
	while(1)
	{
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

static void LogDemo_readFromLog(void)
{
	unsigned int i, entrynumber;
	LogStats demolog_stats;
	LogReader demolog_reader1, demolog_reader2;
	LogEntry* demolog_scratch1, *demolog_scratch2;

	// Allocate space for log entry storage, showing two different ways to calculate the size
	demolog_scratch1 = malloc(sizeof(LogEntry) + sizeof(datablock));

	Log_getStats(demolog_id, &demolog_stats);
	demolog_scratch2 = malloc(demolog_stats.entrySize);

	// Read a single entry from a log
	Log_getLastEntryNumber(demolog_id, &entrynumber);
	Log_readOnce(demolog_id, entrynumber - (demolog_stats.entriesInRAM - 1), demolog_scratch1);
	printf("\nOldest log entry in RAM (%d): %c\n", demolog_scratch1->entryNumber, ((datablock*)(demolog_scratch1->data))->letter);

	// Read the latest entry from a log
	Log_readLastEntry(demolog_id, demolog_scratch2);
	printf("\nLast log entry (%d): %c\n", demolog_scratch2->entryNumber, ((datablock*)(demolog_scratch2->data))->letter);

	// Verify a CRC
	printf("CRC of last log entry is: ");
	if(LogEntry_verifyCRC(demolog_scratch2, demolog_stats.entrySize) == TRUE)
	{
		printf("OK\n");
	}
	else
	{
		printf("not OK!\n");
	}

	// Read a sequence of log entries
	printf("\nRead a sequence of 10 log entries:\n");

	Log_getStats(demolog_id, &demolog_stats);
	Log_getLastEntryNumber(demolog_id, &entrynumber);

	// Start at the last entry
	Log_readStart(demolog_id, entrynumber, &demolog_reader1, demolog_scratch1);

	// Start somewhere in the middle
	Log_readStart(demolog_id, entrynumber - (demolog_stats.entriesInFile / 2),
					&demolog_reader2, demolog_scratch2);

	// Log_readStart already returns the first entry so we can print it
	printf("Log entry - rev: [%d] %c - fwd: [%d] %c\n", demolog_scratch1->entryNumber,
			((datablock*)(demolog_scratch1->data))->letter, demolog_scratch2->entryNumber,
			((datablock*)(demolog_scratch2->data))->letter);

	// Then do 9 more
	for(i = 0; i < 9; i++)
	{
		// Read backwards and read 1 out of every 2 entries
		Log_readPrevious(&demolog_reader1, demolog_scratch1);
		Log_readPrevious(&demolog_reader1, demolog_scratch1);

		// Read forward
		Log_readNext(&demolog_reader2, demolog_scratch2);

		printf("Log entry - rev: [%d] %c - fwd: [%d] %c\n", demolog_scratch1->entryNumber,
				((datablock*)(demolog_scratch1->data))->letter, demolog_scratch2->entryNumber,
				((datablock*)(demolog_scratch2->data))->letter);
	}

	// Stop the readers
	Log_readStop(&demolog_reader1);
	Log_readStop(&demolog_reader2);

	// Free the entries used for temporary storage
	free(demolog_scratch1);
	free(demolog_scratch2);
}

// Example of a typical log initialization structure
static LogInit demolog_init = { .entryDataSize = sizeof(datablock),
									.maxEntriesInRAM = 10,
									.maxEntriesInFRAM = 30,
									.maxEntriesInFile = 80,
									.logFilePath = "a:/logdemo.log", // File is stored on SD-card 0
									.fileWriteVerify = FALSE,
									.fileFlushOnAdd = TRUE,
									.FRAMlogBaseAddress = 0x3000 };

void LogDemo(Boolean newlog)
{
	int result;

	if(newlog == TRUE)
	{
		printf("Starting new log...\n");
	}
	else
	{
		printf("Resuming existing log...\n");
	}

	// Start the log and prints its statistics
	result = Log_start(&demolog_init, newlog, &demolog_id);

	if(result < 0)
	{
		printf("Error starting log, error code: %d\n", result);
	}

	LogDemo_printLogStats(demolog_id);

	// Start the LogAdder task
	xTaskCreate(LogDemo_taskLogAdder, (const signed char*)"LogAdder", 1024, NULL, configMAX_PRIORITIES - 2, NULL);

	// Wait for some entries to be added to the log and the print statistics
	printf("\nWaiting 5 seconds for log entries to be added...\n");
	vTaskDelay(5000 / portTICK_RATE_MS);
	LogDemo_printLogStats(demolog_id);

	// Read entries from log
	LogDemo_readFromLog();

	// Wait for some entries to be added to the log and the print statistics
	printf("\nWaiting 5 seconds for log entries to be added...\n");
	vTaskDelay(5000 / portTICK_RATE_MS);

	// Stop the LogAdder task
	logadder_active = FALSE;
	vTaskDelay(1000 / portTICK_RATE_MS);

	LogDemo_printLogStats(demolog_id);

	// Read entries from log
	LogDemo_readFromLog();

	printf("\n\n");
	printf("End of Log demo, restart required\n\n");
}
