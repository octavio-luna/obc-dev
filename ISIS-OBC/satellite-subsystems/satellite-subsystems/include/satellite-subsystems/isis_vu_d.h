/*
 * isis_vu_d.h
 *
 * AUTOGENERATED CODE
 * Please do not perform manual edits
 * Generated using autogen v0.17.0
 *
 * Generated ICD version: 1.5
 * Generated from:
 *  - isis_vu_rx_d.yaml
 *  - isis_vu_tx_d.yaml
 */

#ifndef ISIS_VU_D_H_
#define ISIS_VU_D_H_

#include "isis_vu_d_types.h"
#include <satellite-subsystems/common_types.h>

/*!
 *	Initialize driver for a number of ISIS_VU_D instances. The first instance can then be referenced by using index 0, the second by using index 1, etc.
 *
 *	@param[in] isis_vu_d Pointer to array of ISIS_VU_D instances
 *	@param[in] isis_vu_dCount Number of ISIS_VU_D instances pointed to by isis_vu_d input parameter
 * 	@return Error code as specified in common_types.h
 */
driver_error_t ISIS_VU_D_Init(const ISIS_VU_D_t* isis_vu_d, uint8_t isis_vu_dCount);

/*!
 * Clears any beacon that is currently set in the transceiver.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__clear_beacon(uint8_t index);

/*!
 * Get frame data from receive buffer
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[out] data_out Pointer to buffer for the response from subsystem
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__get_frame(uint8_t index, isis_vu_d__get_frame__from_t *response);

/*!
 * Get frame data from receive buffer
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[out] data_out Pointer to buffer for the response from subsystem
 * @param[in] data_length Number of elements to read for output struct member 'data' in bytes
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__get_frame_variable(uint8_t index, isis_vu_d__get_frame__from_t *response, size_t data_length);

/*!
 * Retrieves the number of frames that are currently stored in the receiver buffer.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[out] frame_count_out Number of frames in receive buffer
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__get_frame_count(uint8_t index, uint16_t *frame_count_out);

/*!
 * Get full frame from receive buffer (added in TRXVU v1.1.5)
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[out] data_out Pointer to buffer for the response from subsystem
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__get_full_frame(uint8_t index, isis_vu_d__get_full_frame__from_t *response);

/*!
 * Get full frame from receive buffer (added in TRXVU v1.1.5)
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[out] data_out Pointer to buffer for the response from subsystem
 * @param[in] data_length Number of elements to read for output struct member 'data' in bytes
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__get_full_frame_variable(uint8_t index, isis_vu_d__get_full_frame__from_t *response, size_t data_length);

/*!
 * Measure all telemetry channels
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[out] response Struct with response from subsystem
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__get_rx_telemetry(uint8_t index, isis_vu_d__get_rx_telemetry__from_t *response);

/*!
 * Measure all the telemetry channels
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[out] response Struct with response from subsystem
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__get_tx_telemetry(uint8_t index, isis_vu_d__get_tx_telemetry__from_t *response);

/*!
 * Get the stored telemetry channels during the last transmission
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[out] response Struct with response from subsystem
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__get_tx_telemetry_last(uint8_t index, isis_vu_d__get_tx_telemetry_last__from_t *response);

/*!
 * Removes the oldest frame from the receive buffer. This is the same frame that can be retrieved from the receiver buffer command. If there are 0 frames in the receive buffer this command has no effect.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__remove_frame(uint8_t index);

/*!
 * Power cycles the full board (transmitter and receiver will be both reset).
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__reset_hw_rx(uint8_t index);

/*!
 * Power cycles the full board (transmitter and receiver will be both reset).
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__reset_hw_tx(uint8_t index);

/*!
 * Performs a software reset of the receiver processor.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__reset_sw_rx(uint8_t index);

/*!
 * Performs a software reset of the receiver processor.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__reset_sw_tx(uint8_t index);

/*!
 * Performs a reset of the I2C watchdog without performing any other operation.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__reset_wdg_rx(uint8_t index);

/*!
 * Performs a reset of the I2C watchdog without performing any other operation.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__reset_wdg_tx(uint8_t index);

/*!
 * Report receiver uptime
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[out] uptime_out Reports the amount of time the transmitter MCU has been active since the last reset, also known as up- time.
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__rx_uptime(uint8_t index, uint32_t *uptime_out);

/*!
 * Transmit an AX.25 message with default callsigns and specified content.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[in] data_in Data to put in the AX.25 message
 * @param[in] data_length Length of data_in parameter in number of elements, maximum value: 235
 * @param[out] slots_available_out Number of the available slots in the transmission buffer of the VU TC after the frame has been added. 0: Full, 255: Was not added
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__send_frame(uint8_t index, const uint8_t* data_in, size_t data_length, uint8_t *slots_available_out);

/*!
 * Transmit an AX.25 message with override callsigns and specified content.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[in] params Struct with parameters for subsystem
 * @param[in] data_length Length of input struct member 'data' in number of elements, maximum value: 235
 * @param[out] slots_available_out Number of the available slots in the transmission buffer of the VU TC after the frame has been added. 0: Full, 255: Was not added
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__send_frame_with_callsigns(uint8_t index, const isis_vu_d__send_frame_with_callsigns__to_t *params, size_t data_length, uint8_t *slots_available_out);

/*!
 * Set the parameters for the AX25 Beacon with default callsigns.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[in] params Struct with parameters for subsystem
 * @param[in] data_length Length of input struct member 'data' in number of elements, maximum value: 235
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__set_beacon(uint8_t index, const isis_vu_d__set_beacon__to_t *params, size_t data_length);

/*!
 * Set the parameters for the AX25 Beacon with override callsigns.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[in] params Struct with parameters for subsystem
 * @param[in] data_length Length of input struct member 'data' in number of elements, maximum value: 235
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__set_beacon_with_callsigns(uint8_t index, const isis_vu_d__set_beacon_with_callsigns__to_t *params, size_t data_length);

/*!
 * Set AX.25 transmission bitrate
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[in] bitrate_in Transmission bitrate.
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__set_bitrate(uint8_t index, isis_vu_d__bitrate_t bitrate_in);

/*!
 * Sets the default AX.25 FROM callsign in the transmitter.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[in] params Struct with parameters for subsystem
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__set_callsign_from(uint8_t index, const isis_vu_d__set_callsign_from__to_t *params);

/*!
 * Sets the default AX.25 TO callsign in the transmitter.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[in] params Struct with parameters for subsystem
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__set_callsign_to(uint8_t index, const isis_vu_d__set_callsign_to__to_t *params);

/*!
 * Sets the state of the transmitter when it's idle.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[in] idle_state_in The idle state of the transmitter to be set.
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__set_idle_state(uint8_t index, isis_vu_d__onoff_t idle_state_in);

/*!
 * Reports general information about the state the transmitter is currently in.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[out] response Struct with response from subsystem
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__state(uint8_t index, isis_vu_d__state__from_t *response);

/*!
 * Reports the amount of time the transmitter MCU has been active since the last reset, also known as up- time.
 *
 * @param[in] index Index of ISIS_VU_D in list provided during driver initialization
 * @param[out] uptime_out Seconds uptime value.
 * @return Error code as specified in common_types.h
 */
driver_error_t isis_vu_d__tx_uptime(uint8_t index, uint32_t *uptime_out);

#endif /* ISIS_VU_D_H_ */

