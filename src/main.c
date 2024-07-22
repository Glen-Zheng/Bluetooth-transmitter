/*
 * Copyright (c) 2018 Henrik Brix Andersen <henrik@brixandersen.dk>
 *
 * SPDX-License-Identifier: Apache-2.0
 */	

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/console/console.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>

//IPC implementation
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


#define SLEEP_TIME_MS	10000
#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
#define COMPANY_ID_CODE 0xffff


static struct bt_le_adv_param *adv_param = 	
	BT_LE_ADV_PARAM(BT_LE_ADV_OPT_NONE,
	BT_GAP_ADV_FAST_INT_MIN_2,
	BT_GAP_ADV_FAST_INT_MAX_2,
	NULL);

//advertisement data
typedef struct data {
	uint16_t company_code; /* Company Identifier Code. */
	uint32_t number;  //16 UUID value
} data_type;

static data_type data = { COMPANY_ID_CODE, 0x0000 };

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
	BT_DATA(BT_DATA_MANUFACTURER_DATA, (unsigned char *)&data, sizeof(data))
};

//scan response data
static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN)
};

int main(void)
{

	int err;
	console_getline_init(); //initialize console for receiving input

	printk("Starting Transmitter Demo\n");

	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return 0;
	}

	printk("Bluetooth initialized\n");
		
	while(1){
		// get new advertisements from terminal
		char *input; //input
		uint8_t x; //dereference pointer
		
		input = (console_getline()); //receive input from terminal, dereference
		printk("%s \n", input); //actual pointer...
		sscanf(input, "%x", &x);
		data.number = x; 
		printk("New Ad:  %x \n", data.number); //print presumed output
		//some weird reason, spitting out ascii characters?

		err = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
		if (err) {
			printk("Advertising failed to start (err %d)\n", err);
			return 0;
		}

		printk("Advertising successfully started: %02x \n", data.number);

		k_msleep(SLEEP_TIME_MS);
		
		err = bt_le_adv_stop();
		if (err) {
			printk("Advertising failed to stop (err %d)\n", err);
			return 0;
		}
		printk("Advertising successfully stopped\n");


	}

	return 0;

}