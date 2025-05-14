#include "SDManager.h"
#include "LogManager.h"



int SDManagerInit()
{
	static const char* initStr = __FUNCTION__;
	static const char* initErrStr = "%s error %s: %d";
	int err;
	F_SPACE p;
	UPLOG_INFO(initStr);
	// initialize memory to be used by hcc
	if( (err=hcc_mem_init()) ) {
		UPLOG_CRIT(initErrStr,initStr,"initializing hcc memory",err);
		return err;
	}
	// initialize hcc
	if( (err=fs_init()) ) {
		UPLOG_CRIT(initErrStr,initStr,"initializing hcc driver",err);
		return err;
	}
	// register this task with hcc
	if( (err=f_enterFS()) ) {
		UPLOG_CRIT(initErrStr,initStr,"registering init task with hcc",err);
		return err;
	}
	// this hcc distribution does not support ram disk
	// if( (err=f_initvolume(0,f_ramdrvinit,F_AUTO_ASSIGN)) ) { // safe init
	//		UPLOG_CRIT(initErrStr,initStr,"initializing volume 0 as ramdrive",err);
	//		return err;
	// }
	//SDManagerShowStatus(0,1,&p);
	if( (err=f_initvolume(1,atmel_mcipdc_initfunc,F_AUTO_ASSIGN)) ) { // safe init
		UPLOG_CRIT(initErrStr,initStr,"initializing volume 1 as SD card",err);
		return err;
	}
	SDManagerShowStatus(1,1,&p);
	if( (err=f_initvolume(2,atmel_mcipdc_initfunc,F_AUTO_ASSIGN)) ) { // safe init
		UPLOG_CRIT(initErrStr,initStr,"initializing volume 2 as SD card",err);
		return err;
	}
	SDManagerShowStatus(2,1,&p);
	//This task is about to end or at least stop using the filesystems, so unregister now.
	f_releaseFS();
	
	return 0;
}

void SDManagerShutDown() {
	UPLOG_INFO(__FUNCTION__);
	// close ramdrive
	// f_delvolume(0);
	// close sd cards
	f_delvolume(1);
	f_delvolume(2);
	// unregister this task from hcc
	f_releaseFS();
	// delete hcc driver
	fs_delete(); 
	// release hcc mem
	hcc_mem_delete(); /* free the memory used by the filesystem */
}


int SDManagerReInit() {
	UPLOG_INFO(__FUNCTION__);
	SDManagerShutDown();
	return SDManagerInit();
}

int SDManagerShowStatus(int drivenum,char doLog,F_SPACE* pspace) {
	const char* ownStr = __FUNCTION__;
	char todel = 0; int err;
	if(! pspace ) {
		pspace = (F_SPACE*)pvPortMalloc(sizeof(F_SPACE));
		todel = 1;
	}
	if( (err=f_getfreespace(drivenum,pspace)) ) {
		if( todel ) vPortFree(pspace);
		UPLOG_CRIT("%s error: %d",ownStr,err);
		return err;
	}
	if( doLog )	UPLOG_INFO("%s vol=%d total=%d free=%d used=%d bad=%d tot_high=%d free_high=%d used_high=%d bad_high=%d",ownStr,
						drivenum,pspace->total,pspace->free,pspace->used,pspace->bad,pspace->total_high,pspace->free_high,pspace->used_high,pspace->bad_high);
	if( todel ) vPortFree(pspace);
	return 0;
}

