#ifndef SDMANAGER_H
#define SDMANAGER_H

#include <hcc/api_fat.h>
#include <hcc/api_hcc_mem.h>
#include <hcc/api_mdriver_atmel_mcipdc.h>
#include <hcc/api_fs_err.h> // error codes


// Initialize driver and volumes.
// Innitialized volumes are 0 ramdisk, 1 first SD card, 2 second SD card
// (this hcc distributioon does not support ramdisk)
int SDManagerInit();
// Shutdown and initialize again. Warning: close all files in all tasks, and unregister all tasks from driver.
int SDManagerReInit();
// Close driver and release resources. Warning: close all files in all tasks, and unregister all tasks from driver.
void SDManagerShutDown();
// get free space and bad sectors in pspace struct. May also print results if dolog!=0
int SDManagerShowStatus(int drivenum,char doLog,F_SPACE* pspace);

// Important, any task willing to operate on filesystems, will need to register itsef
// to the driver using f_enterFS(). If the task is not going to access filesystems anymore
// it should call f_releaseFS()

// Applications can handle these filesystems and their files, using the functions declared
// in hcc/api_fat.h

// TODO:
//    - ver si inicializamos varias particiones en cada SD card o no.
//      conviene como para tener copias de los archivos en diferentes partes de los filesystems.
//    - incorporar indice de crc's de archivos
//    - incorporar manipulacion de archivos con redundancia.

#endif
