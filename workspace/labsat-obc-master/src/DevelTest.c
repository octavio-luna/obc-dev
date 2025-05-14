
#include "ObcGlobals.h"
#include "DevelTest.h"
#include "LogManager.h"
#include "TimerManager.h"
#include "PowerManager.h"
#include "CSPManager.h"
#include "SDManager.h"
#include <freertos/task.h>
#include <csp/csp.h>


#define TEST_TIMER_INTERVAL 15
#define TEST_TIMER_COUNT 10
int testTimerCallback(unsigned int when, void* _privData) {
	static int count = TEST_TIMER_COUNT;
	UPLOG_NOTICE("%s remaining=%d interval=%dsecs",__FUNCTION__,
						 TEST_TIMER_INTERVAL,count);
	return 1;
}

#define TEST_LOCAL_PORT 10
void testCSPTask(void* param) {
	csp_conn_t *conn;
	csp_packet_t *packet;
   /* Create socket with no specific socket options, e.g. accepts CRC32, HMAC, etc. if enabled during compilation */
   csp_socket_t sock = {0};
   /* Bind socket to all ports, e.g. all incoming connections will be handled here */
   csp_bind(&sock, CSP_ANY);
   /* Create a backlog of 10 connections, i.e. up to 10 new connections can be queued */
   csp_listen(&sock, 10);
	UPLOG_NOTICE("%s accepting conections",__FUNCTION__);
   /* Wait for connections and then process packets on the connection */
   while(1) {

      if( (conn=csp_accept(&sock,10000/*msecs*/)) == NULL) continue;
      int packet_dport = csp_conn_dport(conn);
		UPLOG_NOTICE("%s connection accepted at port %d",__FUNCTION__,packet_dport);
      if( packet_dport==TEST_LOCAL_PORT ) {
			packet = csp_buffer_get(100);
			if( !packet ) { UPLOG_ERR("%s Failed to get CSP buffer\n",__FUNCTION__); }
			else {
      		memcpy(packet->data,"Hi!",3); packet->data[3]=0; packet->length = 4; // include the terminating 0
				csp_send(conn, packet); // send packet and then free packet buffer mem
				while( (packet=csp_read(conn,5000/*msecs*/))!=NULL ) { // finish if no ackets received for 5 seconds
            	UPLOG_NOTICE("Packet received on expected port %d: %s\n", packet_dport,(char *)packet->data);
					// just echo the packet back to sender
					csp_send(conn, packet); // send packet and then free packet buffer mem
            	// csp_buffer_free(packet); // no need to free packet, as it was freed after being sent
				}
         }
      } else {
         while( (packet=csp_read(conn,5000/*msecs*/))!=NULL ) { // finish if no ackets received for 5 seconds
				UPLOG_NOTICE("Packet received on unexpected port %d: %s\n", packet_dport,(char *)packet->data);
               /* Call the default CSP service handler, handle pings, buffer use, etc. */
               csp_service_handler(packet); // frees packet after handling
         }
      }

      /* Close current connection */
      csp_close(conn);
   }
	vTaskDelete(NULL);
}

void testPowerManagerCallback(driver_error_t cmderr,unsigned int when,commandRespData* resp) {
	if( cmderr!=driver_error_none ) {
		UPLOG_ERR("%s getSystemStatus driver_erroor=%d",__FUNCTION__,cmderr);
		return;
	}
	char* str = 0;
	PowerManagerPrintRespHdr(&str,resp);
	UPLOG_NOTICE(str);
	vPortFree(str);
	str = 0;
	PowerManagerPrintSysStatus(&str,resp);
	UPLOG_NOTICE(str);
	vPortFree(str);
}


void DevelTestTask(void* param) {
	// test TimerManager
	TimerManagerAdd(0,testTimerCallback,TEST_TIMER_INTERVAL,TEST_TIMER_COUNT,
						 	NULL,"develTimerCallback");
	// test CSPMAnager
	xTaskCreate(testCSPTask,"testCSPTask",basic_STACK_DEPTH,NULL,basic_TASK_PRIORITY,NULL);

	// test PowerManager
	PowerManagerAddRequest(PM_CC_GETSYSTEMSTATUS,0,testPowerManagerCallback,0);

	// test SDManager
	SDManagerShowStatus(1 /*drivenum*/,1 /*doLog*/,0);
	
	// test LogManager
	vTaskDelay(pdMS_TO_TICKS(7000/*msecs*/)); 
	logRotate();
#if (configUSE_TRACE_FACILITY==1)
	UPLOG_TasksStatus();
#endif
	vTaskDelete(NULL);
}


void DevelTestInit() {
	xTaskCreate(DevelTestTask,"DevelTestTask",basic_STACK_DEPTH,NULL,basic_TASK_PRIORITY,NULL);
	
}
