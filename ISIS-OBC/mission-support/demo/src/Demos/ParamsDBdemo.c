#include "ParamsDBdemo.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <mission-support/ParamsDB.h>
#include <stdio.h>

#define DB_FRAMADDRESS	0x0000

/*!
 * Example of an enum
 */
typedef enum _enum_example
{
	state_idle = 0,
	state_rx = 1,
	state_tx = 2
} enum_example;

/*!
 * Example of a structure
 */
typedef struct _struct_example
{
	unsigned char member1;
	unsigned short member2;
	double member3;
} struct_example;

// Example of a parameter database with different types of variables
static const unsigned char charparam = 0x56;
static const unsigned char arrayparam[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
static const int intparam = 1234;
static const float floatparam = 3.14159265359;
static const char StringParam[] = "ParamsDBtest";
static const enum_example enumparam = state_tx;
static const struct_example structparam = { .member1 = 0, .member2 = 15, .member3 = 12.3456 };

// Parameter array needed for initialization of the parameter database
static const Param default_parameters[] =
{
		{ .size = sizeof(charparam), .data = (void*)&charparam },
		{ .size = 6 * sizeof(unsigned char), .data = (void*)arrayparam },
		{ .size = sizeof(intparam), .data = (void*)&intparam },
		{ .size = sizeof(floatparam), .data = (void*)&floatparam },
		{ .size = sizeof(StringParam), .data = (void*)StringParam },
		{ .size = sizeof(enumparam), .data = (void*)&enumparam },
		{ .size = sizeof(structparam), .data = (void*)&structparam }
};

/*!
 * Function that will print all parameter value
 */
static void ParamsDBdemo_printDB(void)
{
	Param retrieve_param;
	unsigned char tempstorage[30];
	float tempfloat;
	struct_example tempstruct;

	retrieve_param.data = (void*)&tempstorage[0];

	printf("Displaying parameter database contents as: [id] value from FRAM --- value from RAM\n");

	retrieve_param.size = sizeof(unsigned char);
	ParamsDB_getParam(&retrieve_param, 0, paramsDB_FRAM);
	printf("[0] 0x%02x --- ", tempstorage[0]);
	ParamsDB_getParam(&retrieve_param, 0, paramsDB_RAM);
	printf("0x%02x\n", tempstorage[0]);

	retrieve_param.size = 6 * sizeof(unsigned char);
	ParamsDB_getParam(&retrieve_param, 1, paramsDB_FRAM);
	printf("[1] 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x --- ", tempstorage[0], tempstorage[1],
				tempstorage[2], tempstorage[3], tempstorage[4], tempstorage[5]);
	ParamsDB_getParam(&retrieve_param, 1, paramsDB_FRAM);
	printf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\n", tempstorage[0], tempstorage[1],
					tempstorage[2], tempstorage[3], tempstorage[4], tempstorage[5]);

	retrieve_param.size = sizeof(int);
	ParamsDB_getParam(&retrieve_param, 2, paramsDB_FRAM);
	printf("[2] %d --- ", *((int*)(&tempstorage[0])));
	ParamsDB_getParam(&retrieve_param, 2, paramsDB_RAM);
	printf("%d\n", *((int*)(&tempstorage[0])));

	retrieve_param.data = (void*)&tempfloat;
	retrieve_param.size = sizeof(float);
	ParamsDB_getParam(&retrieve_param, 3, paramsDB_FRAM);
	printf("[3] %f --- ", tempfloat);
	ParamsDB_getParam(&retrieve_param, 3, paramsDB_RAM);
	printf("%f\n", tempfloat);

	retrieve_param.size = sizeof(StringParam);
	retrieve_param.data = (void*)&tempstorage[0];
	tempstorage[retrieve_param.size] = 0x0;
	ParamsDB_getParam(&retrieve_param, 4, paramsDB_FRAM);
	printf("[4] %s --- ", (char*)&tempstorage[0]);
	ParamsDB_getParam(&retrieve_param, 4, paramsDB_RAM);
	printf("%s\n", (char*)&tempstorage[0]);

	retrieve_param.size = sizeof(enum_example);
	ParamsDB_getParam(&retrieve_param, 5, paramsDB_FRAM);
	printf("[5] %d --- ", tempstorage[0]);
	ParamsDB_getParam(&retrieve_param, 5, paramsDB_RAM);
	printf("%d\n", tempstorage[0]);

	retrieve_param.data = (void*)&tempstruct;
	retrieve_param.size = sizeof(struct_example);
	ParamsDB_getParam(&retrieve_param, 6, paramsDB_FRAM);
	printf("[6] %02x, %d, %02.10f --- ", tempstruct.member1, tempstruct.member2, tempstruct.member3);
	ParamsDB_getParam(&retrieve_param, 6, paramsDB_RAM);
	printf("%02x, %d, %02.10f\n", tempstruct.member1, tempstruct.member2, tempstruct.member3);
}

static void ParamsDBdemo_updateParameterValues(void)
{
	// Please note that memory has to be allocated for each parameter value to be retrieved
	const char newstring[] = "ParamsDBboot";
	float tempfloat;
	struct_example tempstruct;

	Param update_param;

	/* Please note that parameter value retrieval usually uses RAM only since this is the fastest
	 * and RAM will contain a copy of FRAM after start-up.
	 */

	/*
	 * An example of updating a parameter value in both RAM and FRAM
	 * This value change will both be effective immediately and after a reboot
	 */
	update_param.size = sizeof(float);
	update_param.data = (void*)&tempfloat;

	ParamsDB_getParam(&update_param, 3, paramsDB_RAM);
	tempfloat = tempfloat * 2.0;
	ParamsDB_setParam(&update_param, 3, paramsDB_all);

	/* An example of updating a parameter value in RAM only
	 * This value change will be lost after a reboot or reinitialization of the database
	 */
	update_param.size = sizeof(tempstruct);
	update_param.data = (void*)&tempstruct;

	ParamsDB_getParam(&update_param, 6, paramsDB_RAM);
	tempstruct.member3 *= tempstruct.member2;
	ParamsDB_setParam(&update_param, 6, paramsDB_RAM);

	/* An example of updating a parameter value in FRAM only
	 * Please note that this should be used with caution since the changed parameter value will
	 * not be effective immediately but only after a reboot
	 */
	update_param.size = sizeof(newstring);
	update_param.data = (void*)newstring;
	ParamsDB_setParam(&update_param, 4, paramsDB_FRAM);
}

void ParamsDBdemo(void)
{
	ParamsDBconfig dbconfig = { .numberOfParams = sizeof(default_parameters) / sizeof(Param),
								.FRAMaddress = DB_FRAMADDRESS,
								.defaultParamValues = (Param*)default_parameters };

	/* Step 1:
	 * Start with a clean database
	 */
	ParamsDB_start(&dbconfig, paramsDB_forceReset);

	/* Step 2:
	 * Print parameter DB contents from a clean database
	 */
	printf("\nInitial parameter database contents:\n");
	printf("==============================\n");
	ParamsDBdemo_printDB();
	printf("==============================\n");

	vTaskDelay(5000 / portTICK_RATE_MS);

	/* Step 3:
	 * Change a few parameter values
	 */
	printf("\nUpdating a few parameter values...\n");
	ParamsDBdemo_updateParameterValues();

	vTaskDelay(1000 / portTICK_RATE_MS);

	/* Step 4:
	 * Print parameter DB contents of the updated database, which will show effect of different
	 * update targets (RAM, FRAM, or both)
	 */
	printf("\nUpdated parameter database contents:\n");
	printf("==============================\n");
	ParamsDBdemo_printDB();
	printf("==============================\n");

	vTaskDelay(5000 / portTICK_RATE_MS);

	/* Step 5:
	 * Stop and restart the parameter DB to simulate a reset
	 */
	printf("\nRestarting parameter database to simulate iOBC reset...\n");
	ParamsDB_stop();

	// This is the usual way of starting the parameter database if no explicit reset is required
	ParamsDB_start(&dbconfig, paramsDB_resetIfNeeded);

	vTaskDelay(1000 / portTICK_RATE_MS);

	/* Step 6:
	 * Print parameter DB contents of the updated database after reboot, showing which
	 * updated values will and will not be saved over reboots
	 */
	printf("\nParameter database contents after a reboot:\n");
	printf("==============================\n");
	ParamsDBdemo_printDB();
	printf("==============================\n");
	printf("\n\n");
}
