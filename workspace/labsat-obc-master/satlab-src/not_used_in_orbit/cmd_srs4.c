/*
 * Copyright (c) 2016-2021 Satlab A/S <satlab@satlab.com>
 * All rights reserved. Do not distribute without permission.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>

#include <util/colorprint.h>
#include <util/string.h>

#include <util/csp_client_commands.h>
#include <slash/slash.h>

#include <satlab/srs4.h>
#include <prop-client/prop_client_commands.h>

#include <btp/types.h>
#include <btp/cmd_btp.h>

/* Default node and timeout */
static uint8_t srs4_node = SL_SRS4_DEFAULT_ADDRESS;
static uint32_t srs4_timeout = 1000;

slash_command_group(srs4, "SRS-4 subsystem commands");

static int cmd_srs4_node(struct slash *slash)
{
	uint8_t node;
	char *endptr;

	if (slash->argc < 2) {
		UPLOG_NOTICE("Current node address: %hhu\n", srs4_node);
		return 0;
	}

	node = (uint8_t)strtoul(slash->argv[1], &endptr, 10);
	if (*endptr != '\0')
		return SLASH_EUSAGE;

	srs4_node = node;

	return 0;
}
slash_command_sub(srs4, node, cmd_srs4_node, "[address]",
		  "Set/read node address");

static int cmd_srs4_timeout(struct slash *slash)
{
	uint32_t timeout;
	char *endptr;

	if (slash->argc < 2) {
		UPLOG_NOTICE("Current timeout: %"PRIu32"\n", srs4_timeout);
		return 0;
	}

	timeout = (uint32_t)strtoul(slash->argv[1], &endptr, 10);
	if (*endptr != '\0')
		return SLASH_EUSAGE;

	srs4_timeout = timeout;

	return 0;
}
slash_command_sub(srs4, timeout, cmd_srs4_timeout, "[ms]",
		  "Set/read communication timeout");

slash_command_subgroup(srs4, gwdt, "Ground watchdog commands");

static int cmd_srs4_gwdt_reset(struct slash *slash)
{
	int ret;

	ret = sl_srs4_gwdt_reset(srs4_node, srs4_timeout, SL_SRS4_GWDT_RESET_KEY);
	if (ret < 0) {
		UPLOG_ERR("Failed to reset GWDT: %d\n", ret);
		return SLASH_EIO;
	}

	return 0;
}
slash_command_subsub(srs4, gwdt, reset, cmd_srs4_gwdt_reset, NULL,
		     "Reset ground watchdog");

static int cmd_srs4_gwdt_counter(struct slash *slash)
{
	int ret;
	uint32_t counter;

	ret = sl_srs4_gwdt_counter(srs4_node, srs4_timeout, &counter);
	if (ret < 0) {
		UPLOG_ERR("Failed to read GWDT: %d\n", ret);
		return SLASH_EIO;
	}

	UPDEBUG("%"PRIu32" seconds remaining\n", counter);

	return 0;
}
slash_command_subsub(srs4, gwdt, counter, cmd_srs4_gwdt_counter, NULL,
		     "Read ground watchdog counter");

static int cmd_srs4_run(struct slash *slash)
{
	int ret;
	const char *cmd;

	if (slash->argc != 2)
		return SLASH_EUSAGE;

	cmd = slash->argv[1];

	ret = sl_srs4_shell_run(srs4_node, srs4_timeout, cmd);
	if (ret < 0) {
		UPLOG_ERR("Failed to run command: %s\n", strerror(-ret));
		return SLASH_EIO;
	}

	return 0;
}
slash_command_sub(srs4, run, cmd_srs4_run, "<command>",
		  "Run command on remote system");

static int cmd_srs4_tm(struct slash *slash)
{
	int ret;
	struct sl_prop_query query;
	uint8_t txbuf[64], rxbuf[256];
	size_t chunksize = 200;

	uint32_t bootcount, gwdt_counter;
	uint16_t mv, mw;
	int16_t ma, temp;
	float pwr, err, ebn0;
	uint32_t frames;
	char gwdt_string[20];
	bool carrier_lock, frame_lock;

	ret = sl_prop_remote_query_create_static(&query, txbuf, sizeof(txbuf), rxbuf, sizeof(rxbuf));
	if (ret < 0) {
		UPLOG_ERR("Failed to create query\n");
		return SLASH_ENOMEM;
	}

	/* Set chunksize */
	query.chunksize = chunksize;

	/* Fetch entire TM group */
	ret += sl_prop_remote_query_get(&query, SL_SRS4_PROP_TM_GROUP);
	ret += sl_prop_remote_query_get(&query, SL_SRS4_PROP_SYS_BOOTCOUNT);
	ret += sl_prop_remote_query_get(&query, SL_SRS4_PROP_SYS_GWDT_COUNTER);
	ret += sl_prop_remote_query_get(&query, SL_SRS4_PROP_TX_FRAMES);
	ret += sl_prop_remote_query_get(&query, SL_SRS4_PROP_TX_PWR_FWD);
	ret += sl_prop_remote_query_get(&query, SL_SRS4_PROP_RX_FRAMES);
	ret += sl_prop_remote_query_get(&query, SL_SRS4_PROP_RX_RSSI);
	ret += sl_prop_remote_query_get(&query, SL_SRS4_PROP_RX_EBN0);
	ret += sl_prop_remote_query_get(&query, SL_SRS4_PROP_RX_FREQERR);
	ret += sl_prop_remote_query_get(&query, SL_SRS4_PROP_RX_SYMERR);
	ret += sl_prop_remote_query_get(&query, SL_SRS4_PROP_RX_CARRIER_LOCK);
	ret += sl_prop_remote_query_get(&query, SL_SRS4_PROP_RX_FRAME_LOCK);

	/* Check for errors */
	if (ret < 0) {
		UPLOG_ERR("Failed to add properties to query\n");
		return SLASH_ENOMEM;
	}

	/* Send query message */
	ret = sl_prop_remote_query_send(&query, srs4_node, SL_PROP_DEFAULT_PORT, srs4_timeout);
	if (ret < 0) {
		UPLOG_ERR("Failed to send query\n");
		return SLASH_ENOMEM;
	}

	/* System info */
	UPLOG_INFO("System Info:\n");

	UPLOG_INFO("%-15s", "Bootcount");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_SYS_BOOTCOUNT, &bootcount, sizeof(bootcount));
	UPLOG_INFO(" %7"PRIu32, bootcount);
	UPLOG_INFO(" boots\n");
	UPLOG_INFO("%-15s", "GWDT counter");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_SYS_GWDT_COUNTER, &gwdt_counter, sizeof(gwdt_counter));
	sec_to_time_string(gwdt_counter, gwdt_string, sizeof(gwdt_string));
	UPLOG_INFO("%s", gwdt_string);
	UPLOG_INFO(" (%"PRIu32" seconds)\n", gwdt_counter);

	/* Power consumption */
	UPLOG_INFO( "\nPower Rails:\n");

	UPLOG_INFO("%-13s", "VIN");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_VOLT_VIN, &mv, sizeof(mv));
	UPLOG_INFO( " %9.2f", (float)mv);
	UPLOG_INFO(" mV ");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_CUR_VIN, &ma, sizeof(ma));
	UPLOG_INFO( " %8.2f", (float)ma);
	UPLOG_INFO(" mA ");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_POWER_VIN, &mw, sizeof(mw));
	UPLOG_INFO( " %8.2f", (float)mw);
	UPLOG_INFO(" mW\n");

	UPLOG_INFO("%-13s", "3V3");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_VOLT_3V3, &mv, sizeof(mv));
	UPLOG_INFO( " %9.2f", (float)mv);
	UPLOG_INFO(" mV ");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_CUR_3V3, &ma, sizeof(ma));
	UPLOG_INFO( " %8.2f", (float)ma);
	UPLOG_INFO(" mA ");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_POWER_3V3, &mw, sizeof(mw));
	UPLOG_INFO( " %8.2f", (float)mw);
	UPLOG_INFO(" mW\n");

	UPLOG_INFO("%-13s", "1V8");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_VOLT_1V8, &mv, sizeof(mv));
	UPLOG_INFO( " %9.2f", (float)mv);
	UPLOG_INFO(" mV ");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_CUR_1V8, &ma, sizeof(ma));
	UPLOG_INFO( " %8.2f", (float)ma);
	UPLOG_INFO(" mA ");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_POWER_1V8, &mw, sizeof(mw));
	UPLOG_INFO( " %8.2f", (float)mw);
	UPLOG_INFO(" mW\n");

	UPLOG_INFO("%-13s", "1V0");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_VOLT_1V0, &mv, sizeof(mv));
	UPLOG_INFO( " %9.2f", (float)mv);
	UPLOG_INFO(" mV ");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_CUR_1V0, &ma, sizeof(ma));
	UPLOG_INFO( " %8.2f", (float)ma);
	UPLOG_INFO(" mA ");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_POWER_1V0, &mw, sizeof(mw));
	UPLOG_INFO( " %8.2f", (float)mw);
	UPLOG_INFO(" mW\n");

	UPLOG_INFO("%-13s", "PA");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_VOLT_PA, &mv, sizeof(mv));
	UPLOG_INFO( " %9.2f", (float)mv);
	UPLOG_INFO(" mV ");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_CUR_PA, &ma, sizeof(ma));
	UPLOG_INFO( " %8.2f", (float)ma);
	UPLOG_INFO(" mA ");
	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_POWER_PA, &mw, sizeof(mw));
	UPLOG_INFO( " %8.2f", (float)mw);
	UPLOG_INFO(" mW\n");

	/* Temperatures */
	UPLOG_INFO( "\nTemperature Sensors:\n");

	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_TEMP_PSU, &temp, sizeof(temp));
	UPLOG_INFO("%-15s", "PSU");
	UPLOG_INFO( " %7.2f", temp/100.0);
	UPLOG_INFO(" C\n");

	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_TEMP_MCU, &temp, sizeof(temp));
	UPLOG_INFO("%-15s", "MCU");
	UPLOG_INFO( " %7.2f", temp/100.0);
	UPLOG_INFO(" C\n");

	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_TEMP_FPGA, &temp, sizeof(temp));
	UPLOG_INFO("%-15s", "FPGA");
	UPLOG_INFO( " %7.2f", temp/100.0);
	UPLOG_INFO(" C\n");

	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_TEMP_XCVR, &temp, sizeof(temp));
	UPLOG_INFO("%-15s", "XCVR");
	UPLOG_INFO( " %7.2f", temp/100.0);
	UPLOG_INFO(" C\n");

	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_TEMP_LNA, &temp, sizeof(temp));
	UPLOG_INFO("%-15s", "LNA");
	UPLOG_INFO( " %7.2f", temp/100.0);
	UPLOG_INFO(" C\n");

	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TM_TEMP_PA, &temp, sizeof(temp));
	UPLOG_INFO("%-15s", "PA");
	UPLOG_INFO( " %7.2f", temp/100.0);
	UPLOG_INFO(" C\n");

	/* Transmit */
	UPLOG_INFO( "\nTransmitter:\n");

	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TX_FRAMES, &frames, sizeof(frames));
	UPLOG_INFO("%-15s", "Frames");
	UPLOG_INFO( " %7"PRIu32, frames);
	UPLOG_INFO(" frames\n");

	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_TX_PWR_FWD, &pwr, sizeof(pwr));
	UPLOG_INFO("%-15s", "Forward power");
	UPLOG_INFO( " %7.2f", pwr);
	UPLOG_INFO(" dBm\n");

	/* Receive */
	UPLOG_INFO( "\nReceiver:\n");

	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_RX_FRAMES, &frames, sizeof(frames));
	UPLOG_INFO("%-15s", "Frames");
	UPLOG_INFO( " %7"PRIu32, frames);
	UPLOG_INFO(" frames\n");

	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_RX_RSSI, &pwr, sizeof(pwr));
	UPLOG_INFO("%-15s", "RSSI");
	UPLOG_INFO( " %7.2f", pwr);
	UPLOG_INFO(" dBm\n");

	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_RX_EBN0, &ebn0, sizeof(ebn0));
	UPLOG_INFO("%-15s", "Eb/N0");
	UPLOG_INFO( " %7.2f", ebn0);
	UPLOG_INFO(" dB\n");

	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_RX_FREQERR, &err, sizeof(err));
	UPLOG_INFO("%-14s", "Freq error");
	UPLOG_INFO( " %8.2f", err);
	UPLOG_INFO(" Hz\n");

	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_RX_SYMERR, &err, sizeof(err));
	UPLOG_INFO("%-15s", "Rate error");
	UPLOG_INFO( " %7.2f", err);
	UPLOG_INFO(" ppm\n");

	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_RX_CARRIER_LOCK, &carrier_lock, sizeof(carrier_lock));
	UPLOG_INFO("%-15s", "Carrier sync");
	if (carrier_lock)
		UPLOG_INFO( "%9s", "locked\n");
	else
		UPLOG_INFO( "%9s", "unlocked\n");

	sl_prop_remote_query_get_reply(&query, SL_SRS4_PROP_RX_FRAME_LOCK, &frame_lock, sizeof(frame_lock));
	UPLOG_INFO("%-15s", "Frame sync");
	if (frame_lock)
		UPLOG_INFO( "%9s", "locked\n");
	else
		UPLOG_INFO( "%9s", "unlocked\n");

	sl_prop_remote_query_destroy(&query);

	return 0;
}
slash_command_sub(srs4, tm, cmd_srs4_tm, NULL,
		  "Show telemetry data");

/* Bootloader commands */
slash_command_subgroup(srs4, boot, "Bootloader commands");

static int cmd_srs4_boot_flash(struct slash *slash)
{
	int ret;
	const char *filename;
	uint32_t crc;

	if (slash->argc < 2)
		return SLASH_EUSAGE;

	filename = slash->argv[1];

	UPLOG_INFO("Flashing \"%s\" to alternate boot partition\n", filename);

	ret = sl_srs4_bootloader_flash_alternate(srs4_node, srs4_timeout, filename, &crc);
	if (ret < 0) {
		UPLOG_ERR("Failed to flash image: %s\n", strerror(-ret));
		return SLASH_EIO;
	}

	UPLOG_INFO("Flashed image, checksum = %08"PRIx32"\n", crc);

	return 0;
}
slash_command_subsub(srs4, boot, flash, cmd_srs4_boot_flash, "<filename>",
		     "Flash image from NOR-flash to alternate partition");

static int cmd_srs4_boot_erase(struct slash *slash)
{
	int ret;

	ret = sl_srs4_bootloader_erase_alternate(srs4_node, srs4_timeout);
	if (ret < 0) {
		UPLOG_ERR("Failed to erase image: %s\n", strerror(-ret));
		return SLASH_EIO;
	}

	UPLOG_INFO("Success - alternate image was erased\n");

	return 0;
}
slash_command_subsub(srs4, boot, erase, cmd_srs4_boot_erase,
		     NULL, "Erase alternate partition");

static int cmd_srs4_boot_checksum(struct slash *slash)
{
	int ret;
	uint32_t size, crc;
	char *endptr;

	if (slash->argc < 2)
		return SLASH_EUSAGE;

	size = (uint32_t)strtoul(slash->argv[1], &endptr, 10);
	if (*endptr != '\0')
		return SLASH_EUSAGE;

	ret = sl_srs4_bootloader_checksum_alternate(srs4_node, srs4_timeout, size, &crc);
	if (ret < 0) {
		UPLOG_ERR("Failed to checksum image: %s\n", strerror(-ret));
		return SLASH_EIO;
	}

	UPLOG_INFO("Image checksum = %08"PRIx32"\n", crc);

	return 0;
}
slash_command_subsub(srs4, boot, checksum, cmd_srs4_boot_checksum,
		     "<size>", "Calculate checksum of alternate partition");

static int cmd_srs4_boot_verify(struct slash *slash)
{
	int ret;
	uint8_t partition = 1;
	uint32_t size, checksum;

	if (slash->argc > 1) {
		if (!strcmp(slash->argv[1], "primary")) {
			partition = 0;
		} else if (!strcmp(slash->argv[1], "alternate")) {
			partition = 1;
		} else {
			return SLASH_EUSAGE;
		}
	}

	ret = sl_srs4_bootloader_verify(srs4_node, srs4_timeout, partition, &size, &checksum);
	if (ret < 0) {
		UPLOG_ERR("Failed to verify image: %s\n", strerror(-ret));
		return SLASH_EIO;
	}

	UPLOG_INFO("%s image is %s! (size %u, checksum %08X)\n",
	       partition == 0 ? "Primary" : "Alternate",
	       ret == 0 ? "OK" : "corrupt",
	       size, checksum);

	return ret == 0 ? 0 : SLASH_EIO;
}
slash_command_subsub(srs4, boot, verify, cmd_srs4_boot_verify,
		     "[partition]", "Verify image in partition");

static int cmd_srs4_boot_set(struct slash *slash)
{
	int ret;
	uint8_t boots = 1;
	unsigned long boots_parsed;
	char *endptr;

	if (slash->argc > 1) {
		boots_parsed = strtoul(slash->argv[1], &endptr, 10);
		if (*endptr != '\0')
			return SLASH_EUSAGE;

		if (boots_parsed > 255) {
			UPLOG_ERR("Maximum number of boots is 255\n");
			return SLASH_EUSAGE;
		}

		boots = boots_parsed;
	}

	UPLOG_INFO("Setting alternate firmware boot for %hhu boots\n", boots);

	ret = sl_srs4_bootloader_set_alternate(srs4_node, srs4_timeout, boots);
	if (ret < 0) {
		UPLOG_INFO("Failed to set boot image: %s\n", strerror(-ret));
		return SLASH_EIO;
	}

	UPLOG_INFO("Success - Reboot to boot alternate image\n");

	return 0;
}
slash_command_subsub(srs4, boot, set, cmd_srs4_boot_set,
		     "[boots]", "Set number of boots on alternate partition");

/* CSP commands */
SL_CSP_CLIENT_COMMANDS_GENERATE(srs4, &srs4_node, &srs4_timeout);

/* Properties commands */
SL_PROP_CLIENT_COMMANDS_GENERATE(srs4, &srs4_node, SL_PROP_DEFAULT_PORT, &srs4_timeout, &srs4_props);

/* BTP commands */
SL_BTP_CLIENT_COMMANDS_GENERATE(srs4, &srs4_node, BTP_DEFAULT_PORT);
