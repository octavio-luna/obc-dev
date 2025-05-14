#include <PowerManager.h>

extern uint8_t piu_index;

driver_error_t uart_piu__getsystemstatus(isismepsv2_ivid7_piu__getsystemstatus__from_t* response) {
	return driver_error_none;
}
driver_error_t uart_piu__resetwatchdog(isismepsv2_ivid7_piu__replyheader_t* response) {
	return driver_error_none;
}
driver_error_t uart_piu__correcttime(int diff,isismepsv2_ivid7_piu__replyheader_t* reply) {
	return driver_error_none;
}
driver_error_t uart_piu__outputbuschannelon(isismepsv2_ivid7_piu__imeps_channel_t channel_idx,isismepsv2_ivid7_piu__replyheader_t* response) {
	return driver_error_none;
}
driver_error_t uart_piu__outputbuschanneloff(isismepsv2_ivid7_piu__imeps_channel_t channel_idx,isismepsv2_ivid7_piu__replyheader_t* response) {
	return driver_error_none;
}
driver_error_t uart_piu__outputbusgroupon(isismepsv2_ivid7_piu__outputbusgroupon__to_t* obusOn,isismepsv2_ivid7_piu__replyheader_t* response) {
	return driver_error_none;
}
driver_error_t uart_piu__outputbusgroupoff(isismepsv2_ivid7_piu__outputbusgroupoff__to_t* obusOff,isismepsv2_ivid7_piu__replyheader_t* response) {
	return driver_error_none;
}
driver_error_t uart_piu__outputbusgroupstate(isismepsv2_ivid7_piu__outputbusgroupstate__to_t* obusState,isismepsv2_ivid7_piu__replyheader_t* response) {
	return driver_error_none;
}
driver_error_t uart_piu__setconfigurationparameter(isismepsv2_ivid7_piu__setconfigurationparameter__to_t* setConfParam,isismepsv2_ivid7_piu__setconfigurationparameter__from_t* response) {
	return driver_error_none;
}
