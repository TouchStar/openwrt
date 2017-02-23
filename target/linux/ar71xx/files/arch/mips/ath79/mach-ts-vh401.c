/*
 *  TouchStar VH401 board support
 *
 *  Copyright (C) 2016 David Thornley <david.thornley@touchstargroup.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
*/

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>
#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#include <linux/gpio.h>
#include <linux/delay.h>

//#define FAKE_TPLINK

#define TS_VH401_GPIO_LED_SYSTEM		   	1

#ifdef FAKE_TPLINK
#define TS_VH401_GPIO_PIN_USBPOWER      	8		// Just to get working on TPLink.
#endif
#define TS_VH401_GPIO_BTN_RESET	   	   		11
#define TS_VH401_GPIO_CELLULAR_RESET   		14

#define TS_VH401_GPIO_CELLULAR_CINTERION_PH8_POWERON_PERIOD		150		// Per specification (>3.001 firmware) must be >100ms

#define TS_VH401_ART_DATA_ADDR				0x1fff0000

#define TS_VH401_KEYS_POLL_INTERVAL   		20	/* msecs */
#define TS_VH401_KEYS_DEBOUNCE_INTERVAL		(3 * TS_VH401_KEYS_POLL_INTERVAL)

#define TS_VH401_MAC0_OFFSET				0x0000
#define TS_VH401_MAC1_OFFSET				0x0006
#define TS_VH401_CALDATA_OFFSET				0x1000
#define TS_VH401_WMAC_MAC_OFFSET			0x1002

static struct gpio_led ts_VH401_leds_gpio[] __initdata = {
	{
		.name = "ts-vh401:system",
		.gpio = TS_VH401_GPIO_LED_SYSTEM,
		.active_low = 1,
	}
};

static struct gpio_keys_button ts_VH401_gpio_keys[] __initdata = {
	{
		.desc = "reset",
		.type = EV_KEY,
		.code = KEY_RESTART,
		.debounce_interval = TS_VH401_KEYS_DEBOUNCE_INTERVAL,
		.gpio = TS_VH401_GPIO_BTN_RESET,
		.active_low = 1,
	},
};

static void __init ts_vh401_setup(void)
{
	/* ART(cal_data) base address */
	u8 *art = (u8 *) KSEG1ADDR(TS_VH401_ART_DATA_ADDR);
	
	/* register flash. */
	ath79_register_m25p80(NULL);
	ath79_register_wmac(art + TS_VH401_CALDATA_OFFSET,
					    art + TS_VH401_WMAC_MAC_OFFSET);

	/* disable PHY_SWAP and PHY_ADDR_SWAP bits */
	//ath79_setup_ar933x_phy4_switch(false, false);
	ath79_setup_ar933x_phy4_switch(true, true);

	ath79_init_mac(ath79_eth0_data.mac_addr, art+TS_VH401_MAC0_OFFSET, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, art+TS_VH401_MAC1_OFFSET, 0);
	ath79_register_mdio(0, 0x0);
	ath79_register_eth(0);
	ath79_register_eth(1);

	ath79_gpio_function_disable(AR724X_GPIO_FUNC_ETH_SWITCH_LED0_EN |
								AR724X_GPIO_FUNC_ETH_SWITCH_LED1_EN |
								AR724X_GPIO_FUNC_ETH_SWITCH_LED2_EN |
								AR724X_GPIO_FUNC_ETH_SWITCH_LED3_EN |
								AR724X_GPIO_FUNC_ETH_SWITCH_LED4_EN);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(ts_VH401_leds_gpio),
								ts_VH401_leds_gpio);
	ath79_register_gpio_keys_polled(-1, TS_VH401_KEYS_POLL_INTERVAL,
								ARRAY_SIZE(ts_VH401_gpio_keys),
								ts_VH401_gpio_keys);

	gpio_request_one(TS_VH401_GPIO_CELLULAR_RESET,
			 GPIOF_OUT_INIT_HIGH | GPIOF_EXPORT_DIR_FIXED,
			 "Cellular Reset");

		// Pulse the modem for initial power-on.
	gpio_set_value(TS_VH401_GPIO_CELLULAR_RESET, 0);
	mdelay(TS_VH401_GPIO_CELLULAR_CINTERION_PH8_POWERON_PERIOD);
	gpio_set_value(TS_VH401_GPIO_CELLULAR_RESET, 1);

#ifdef FAKE_TPLINK	
	/* enable usb for TPLink boards*/
	gpio_request_one(TS_VH401_GPIO_PIN_USBPOWER,
				 		GPIOF_OUT_INIT_HIGH | GPIOF_EXPORT_DIR_FIXED,
	 			 		"USB power");
#endif

	ath79_register_usb();
}

MIPS_MACHINE(ATH79_MACH_TS_VH401, "TS-VH401", "TOUCHSTAR VH401", ts_vh401_setup);
