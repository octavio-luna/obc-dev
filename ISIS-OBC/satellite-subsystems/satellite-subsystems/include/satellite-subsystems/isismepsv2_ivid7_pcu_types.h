/*
 * isismepsv2_ivid7_pcu_types.h
 *
 * AUTOGENERATED CODE
 * Please do not perform manual edits
 * Generated using autogen v0.19.2
 *
 * Generated from:
 *  - imepsv2_structs.yaml
 *  - imepsv2_ivid7_pcu.yaml
 */

#ifndef ISISMEPSV2_IVID7_PCU_TYPES_H_
#define ISISMEPSV2_IVID7_PCU_TYPES_H_

#include <stddef.h>
#include <stdint.h>

/* DEFINES */

#define ISISMEPSV2_IVID7_PCU_NOP_ID {0x13, 0x07, 0x02, 0x00}
#define ISISMEPSV2_IVID7_PCU_CANCEL_ID {0x13, 0x07, 0x04, 0x00}
#define ISISMEPSV2_IVID7_PCU_RESETWATCHDOG_ID {0x13, 0x07, 0x06, 0x00}
#define ISISMEPSV2_IVID7_PCU_GETSYSTEMSTATUS_ID {0x13, 0x07, 0x40, 0x00}
#define ISISMEPSV2_IVID7_PCU_GETHOUSEKEEPINGDATARAW_ID {0x13, 0x07, 0x70, 0x00}
#define ISISMEPSV2_IVID7_PCU_GETHOUSEKEEPINGDATAENG_ID {0x13, 0x07, 0x72, 0x00}
#define ISISMEPSV2_IVID7_PCU_GETHOUSEKEEPINGDATARUNNINGAVG_ID {0x13, 0x07, 0x74, 0x00}
#define ISISMEPSV2_IVID7_PCU_GETCONFIGURATIONPARAMETER_ID {0x13, 0x07, 0x82, 0x00}
#define ISISMEPSV2_IVID7_PCU_SETCONFIGURATIONPARAMETER_ID {0x13, 0x07, 0x84, 0x00}
#define ISISMEPSV2_IVID7_PCU_RESETCONFIGURATIONPARAMETER_ID {0x13, 0x07, 0x86, 0x00}
#define ISISMEPSV2_IVID7_PCU_RESETCONFIGURATION_ID {0x13, 0x07, 0x90, 0x00}
#define ISISMEPSV2_IVID7_PCU_LOADCONFIGURATION_ID {0x13, 0x07, 0x92, 0x00}
#define ISISMEPSV2_IVID7_PCU_SAVECONFIGURATION_ID {0x13, 0x07, 0x94, 0x00}
#define ISISMEPSV2_IVID7_PCU_RESET_ID {0x13, 0x07, 0xAA, 0x00}
#define ISISMEPSV2_IVID7_PCU_CORRECTTIME_ID {0x13, 0x07, 0xC4, 0x00}
#define ISISMEPSV2_IVID7_PCU_ZERORESETCAUSECOUNTERS_ID {0x13, 0x07, 0xC6, 0x00}


/* ENUMS */

/* STRUCTS */

/*!
 *  ISISMEPSV2_IVID7_PCU instance structure
 */
typedef struct
{
    uint8_t i2cAddr; /*!< I2C address used for this instance */
} ISISMEPSV2_IVID7_PCU_t;

/*!
 * Union for storing the parameters for struct ReplyHeader.
 */
typedef union __attribute__((__packed__)) _isismepsv2_ivid7_pcu__replyheader_t
{
    unsigned char raw[5];
    struct __attribute__ ((__packed__))
    {
        uint8_t stid; /*!< See "System Type Identifier (STID)" (page 22) */
        uint8_t ivid; /*!< See "Interface Version Identifier (IVID)" (page 23) */
        uint8_t rc; /*!< See "Board Identifier (BID)" (page 24) */
        uint8_t bid; /*!< Response code as stated in the header (inside parenthesis). */
        uint8_t cmderr : 4; /*!< See "Response Status Information (STAT)" (page 25) */
        uint8_t reserved : 3; /*!< See "Response Status Information (STAT)" (page 25) */
        uint8_t new_flag : 1; /*!< See "Response Status Information (STAT)" (page 25) */
    } fields;
} isismepsv2_ivid7_pcu__replyheader_t;

/*!
 * Union for storing the parameters for struct VIPDraw.
 */
typedef union __attribute__((__packed__)) _isismepsv2_ivid7_pcu__vipdraw_t
{
    unsigned char raw[6];
    struct __attribute__ ((__packed__))
    {
        int16_t volt; /*!<  \note conversion: eng. value [in mV] = 0.9765625 * raw + 0*/
        int16_t current; /*!<  \note conversion: eng. value [in mA] = 0.305175781 * raw + 0*/
        int16_t power; /*!<  \note conversion: eng. value [in mW] = 9.765625 * raw + 0*/
    } fields;
} isismepsv2_ivid7_pcu__vipdraw_t;

/*!
 * Union for storing the parameters for struct VIPDeng.
 */
typedef union __attribute__((__packed__)) _isismepsv2_ivid7_pcu__vipdeng_t
{
    unsigned char raw[6];
    struct __attribute__ ((__packed__))
    {
        int16_t volt; /*!<  */
        int16_t current; /*!<  */
        int16_t power; /*!<  \note conversion: eng. value [in mW] = 10 * raw */
    } fields;
} isismepsv2_ivid7_pcu__vipdeng_t;

/*!
 * Union for storing the parameters for struct CCDraw.
 */
typedef union __attribute__((__packed__)) _isismepsv2_ivid7_pcu__ccdraw_t
{
    unsigned char raw[14];
    struct __attribute__ ((__packed__))
    {
        isismepsv2_ivid7_pcu__vipdraw_t vip_cc_output; /*!<  */
        uint16_t volt_in_mppt; /*!<  */
        uint16_t curr_in_mppt; /*!<  */
        uint16_t volt_out_mppt; /*!<  */
        uint16_t curr_out_mppt; /*!<  */
    } fields;
} isismepsv2_ivid7_pcu__ccdraw_t;

/*!
 * Union for storing the parameters for struct CCDeng.
 */
typedef union __attribute__((__packed__)) _isismepsv2_ivid7_pcu__ccdeng_t
{
    unsigned char raw[14];
    struct __attribute__ ((__packed__))
    {
        isismepsv2_ivid7_pcu__vipdeng_t vip_cc_output; /*!<  */
        int16_t volt_in_mppt; /*!<  */
        int16_t curr_in_mppt; /*!<  */
        int16_t volt_out_mppt; /*!<  */
        int16_t curr_out_mppt; /*!<  */
    } fields;
} isismepsv2_ivid7_pcu__ccdeng_t;

/*!
 * Union for storing the parameters received by getconfigurationparameter.
 */
typedef union __attribute__((__packed__)) _isismepsv2_ivid7_pcu__getconfigurationparameter__from_t
{
    unsigned char raw[16];
    struct __attribute__ ((__packed__))
    {
        isismepsv2_ivid7_pcu__replyheader_t reply_header; /*!<  */
        uint8_t reserved; /*!<  */
        uint16_t par_id; /*!< parameter id of the parameter to get */
        uint8_t par_val[8]; /*!< parameter value encompassing n bytes. The length depends on the parameter type. */
    } fields;
} isismepsv2_ivid7_pcu__getconfigurationparameter__from_t;

/*!
 * Union for storing the parameters received by gethousekeepingdataeng.
 */
typedef union __attribute__((__packed__)) _isismepsv2_ivid7_pcu__gethousekeepingdataeng__from_t
{
    unsigned char raw[72];
    struct __attribute__ ((__packed__))
    {
        isismepsv2_ivid7_pcu__replyheader_t reply_header; /*!< "See \""System Type Identifier (STID)\"" (page 22)" */
        uint8_t reserved; /*!< (reserved) */
        int16_t volt_brdsup; /*!< Voltage of internal board supply */
        int16_t temp; /*!< Measured temperature provided by a sensor internal to the MCU \note conversion: eng. value [in °C] = 0.01 * raw */
        isismepsv2_ivid7_pcu__vipdeng_t vip_output; /*!< Input V, I and P data for the unit in raw form. Effectively a sum of all CCx */
        isismepsv2_ivid7_pcu__ccdeng_t cc1; /*!< Data on conditioning chain */
        isismepsv2_ivid7_pcu__ccdeng_t cc2; /*!< Data on conditioning chain */
        isismepsv2_ivid7_pcu__ccdeng_t cc3; /*!< Data on conditioning chain */
        isismepsv2_ivid7_pcu__ccdeng_t cc4; /*!< Data on conditioning chain */
    } fields;
} isismepsv2_ivid7_pcu__gethousekeepingdataeng__from_t;

/*!
 * Union for storing the parameters received by gethousekeepingdataraw.
 */
typedef union __attribute__((__packed__)) _isismepsv2_ivid7_pcu__gethousekeepingdataraw__from_t
{
    unsigned char raw[72];
    struct __attribute__ ((__packed__))
    {
        isismepsv2_ivid7_pcu__replyheader_t reply_header; /*!< "See \""System Type Identifier (STID)\"" (page 22)" */
        uint8_t reserved; /*!< (reserved) */
        uint16_t volt_brdsup; /*!< Voltage of internal board supply in raw form */
        uint16_t temp; /*!< Measured temperature provided by a sensor internal to the MCU in raw form */
        isismepsv2_ivid7_pcu__vipdraw_t vip_output; /*!< Input V, I and P data for the unit in raw form. Effectively a sum of all CCxraw. */
        isismepsv2_ivid7_pcu__ccdraw_t cc1; /*!< Data on conditioning chain in raw form. */
        isismepsv2_ivid7_pcu__ccdraw_t cc2; /*!< Data on conditioning chain in raw form. */
        isismepsv2_ivid7_pcu__ccdraw_t cc3; /*!< Data on conditioning chain in raw form. */
        isismepsv2_ivid7_pcu__ccdraw_t cc4; /*!< Data on conditioning chain in raw form. */
    } fields;
} isismepsv2_ivid7_pcu__gethousekeepingdataraw__from_t;

/*!
 * Union for storing the parameters received by gethousekeepingdatarunningavg.
 */
typedef union __attribute__((__packed__)) _isismepsv2_ivid7_pcu__gethousekeepingdatarunningavg__from_t
{
    unsigned char raw[72];
    struct __attribute__ ((__packed__))
    {
        isismepsv2_ivid7_pcu__replyheader_t reply_header; /*!< "See \""System Type Identifier (STID)\"" (page 22)" */
        uint8_t reserved; /*!< (reserved) */
        int16_t volt_brdsup; /*!< Voltage of internal board supply */
        int16_t temp; /*!< Measured temperature provided by a sensor internal to the MCU \note conversion: eng. value [in °C] = 0.01 * raw */
        isismepsv2_ivid7_pcu__vipdeng_t vip_output; /*!< Input V, I and P data for the unit in raw form. Effectively a sum of all CCx */
        isismepsv2_ivid7_pcu__ccdeng_t cc1; /*!< Data on conditioning chain */
        isismepsv2_ivid7_pcu__ccdeng_t cc2; /*!< Data on conditioning chain */
        isismepsv2_ivid7_pcu__ccdeng_t cc3; /*!< Data on conditioning chain */
        isismepsv2_ivid7_pcu__ccdeng_t cc4; /*!< Data on conditioning chain */
    } fields;
} isismepsv2_ivid7_pcu__gethousekeepingdatarunningavg__from_t;

/*!
 * Union for storing the parameters received by getsystemstatus.
 */
typedef union __attribute__((__packed__)) _isismepsv2_ivid7_pcu__getsystemstatus__from_t
{
    unsigned char raw[36];
    struct __attribute__ ((__packed__))
    {
        isismepsv2_ivid7_pcu__replyheader_t reply_header; /*!<  */
        uint8_t mode; /*!< Indicates the current mode of the system. */
        uint8_t conf; /*!< Read/write configuration parameters have been changed by the EPS master since the last parameters load/save operation */
        uint8_t reset_cause; /*!< Cause of last reset. */
        uint32_t uptime; /*!<  */
        uint16_t error; /*!<  */
        uint16_t rc_cnt_pwron; /*!<  */
        uint16_t rc_cnt_wdg; /*!<  */
        uint16_t rc_cnt_cmd; /*!<  */
        uint16_t rc_cnt_pweron_mcu; /*!<  */
        uint16_t rc_cnt_emlopo; /*!<  */
        uint16_t prevcmd_elapsed; /*!<  */
        uint32_t unix_time; /*!<  */
        uint8_t unix_year; /*!<  \note conversion: eng. value = raw+ 2000*/
        uint8_t unix_month; /*!<  */
        uint8_t unix_day; /*!<  */
        uint8_t unix_hour; /*!<  */
        uint8_t unix_minute; /*!<  */
        uint8_t unix_second; /*!<  */
    } fields;
} isismepsv2_ivid7_pcu__getsystemstatus__from_t;

/*!
 * Union for storing the parameters received by resetconfigurationparameter.
 */
typedef union __attribute__((__packed__)) _isismepsv2_ivid7_pcu__resetconfigurationparameter__from_t
{
    unsigned char raw[16];
    struct __attribute__ ((__packed__))
    {
        isismepsv2_ivid7_pcu__replyheader_t reply_header; /*!<  */
        uint8_t reserved; /*!<  */
        uint16_t par_id; /*!< parameter id of the parameter to get */
        uint8_t par_val[8]; /*!< parameter value encompassing n bytes. The length depends on the parameter type. */
    } fields;
} isismepsv2_ivid7_pcu__resetconfigurationparameter__from_t;

/*!
 * Union for storing the parameters sent by setconfigurationparameter.
 */
typedef union __attribute__((__packed__)) _isismepsv2_ivid7_pcu__setconfigurationparameter__to_t
{
    unsigned char raw[10];
    struct __attribute__ ((__packed__))
    {
        uint16_t par_id; /*!< parameter id of the parameter to get */
        uint8_t par_val[8]; /*!< new parameter value encompassing n bytes. The length depends on the parameter type. */
    } fields;
} isismepsv2_ivid7_pcu__setconfigurationparameter__to_t;

/*!
 * Union for storing the parameters received by setconfigurationparameter.
 */
typedef union __attribute__((__packed__)) _isismepsv2_ivid7_pcu__setconfigurationparameter__from_t
{
    unsigned char raw[16];
    struct __attribute__ ((__packed__))
    {
        isismepsv2_ivid7_pcu__replyheader_t reply_header; /*!<  */
        uint8_t reserved; /*!<  */
        uint16_t par_id; /*!< parameter id of the parameter to get */
        uint8_t par_val[8]; /*!< parameter value encompassing n bytes. The length depends on the parameter type. */
    } fields;
} isismepsv2_ivid7_pcu__setconfigurationparameter__from_t;

#endif /* ISISMEPSV2_IVID7_PCU_TYPES_H_ */

