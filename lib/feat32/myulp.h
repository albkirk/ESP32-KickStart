#include "esp32/ulp.h"
#include "driver/rtc_io.h"


static const int RTC_GPIO_channels[] = { 
        RTCIO_GPIO0_CHANNEL ,        //RTCIO_CHANNEL_11
		-1,
        RTCIO_GPIO2_CHANNEL ,        //RTCIO_CHANNEL_12
		-1,
        RTCIO_GPIO4_CHANNEL ,        //RTCIO_CHANNEL_10
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
        RTCIO_GPIO12_CHANNEL,        //RTCIO_CHANNEL_15
        RTCIO_GPIO13_CHANNEL,        //RTCIO_CHANNEL_14
        RTCIO_GPIO14_CHANNEL,        //RTCIO_CHANNEL_16
        RTCIO_GPIO15_CHANNEL,        //RTCIO_CHANNEL_13
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
        RTCIO_GPIO25_CHANNEL,        //RTCIO_CHANNEL_6
        RTCIO_GPIO26_CHANNEL,        //RTCIO_CHANNEL_7
        RTCIO_GPIO27_CHANNEL,        //RTCIO_CHANNEL_17
		-1,
		-1,
		-1,
		-1,
        RTCIO_GPIO32_CHANNEL,        //RTCIO_CHANNEL_9
        RTCIO_GPIO33_CHANNEL,        //RTCIO_CHANNEL_8
        RTCIO_GPIO34_CHANNEL,        //RTCIO_CHANNEL_4
        RTCIO_GPIO35_CHANNEL,        //RTCIO_CHANNEL_5
        RTCIO_GPIO36_CHANNEL,        //RTCIO_CHANNEL_0
        RTCIO_GPIO37_CHANNEL,        //RTCIO_CHANNEL_1
        RTCIO_GPIO38_CHANNEL,        //RTCIO_CHANNEL_2
        RTCIO_GPIO39_CHANNEL,        //RTCIO_CHANNEL_3
};

// Slow memory variable assignment
enum {
  SLOW_BLINK_EN,                                // LED Blink Enabled
  SLOW_GPS_POWER,                               // GPS Power Switch/Status

  SLOW_PROG_ADDR                                // Program start address
};

void ulp_action(uint32_t us) {
  // Set ULP activation interval
    ulp_set_wakeup_period(0, us);

  // Slow memory initialization
    memset(RTC_SLOW_MEM, 0, 8192);

  // status initialization
    RTC_SLOW_MEM[SLOW_BLINK_EN] = 0;            // Enable/Disable LED Blink
    RTC_SLOW_MEM[SLOW_GPS_POWER] = 1;           // Enable GPS Switch

  // PIN to GPS Switch
    const gpio_num_t rtc_pin = (gpio_num_t)GPS_SW;          // Ex.: GPIO_NUM_12
    const int rtc_pin_bit = RTC_GPIO_channels[GPS_SW] + 14;
  

  // GPIO initialization (set to output and initial value is 0)
    rtc_gpio_init(rtc_pin);
    rtc_gpio_set_direction(rtc_pin, RTC_GPIO_MODE_OUTPUT_ONLY);
    rtc_gpio_set_level(rtc_pin, 1);

  // ULP Program
    const ulp_insn_t  ulp_prog[] = {
        I_MOVI(R3, SLOW_GPS_POWER),             // R3 = SLOW_GPS_POWER
        I_LD(R0, R3, 0),                        // R0 = RTC_SLOW_MEM[R3(SLOW_GPS_POWER)]
        M_BL(1, 1),                             // IF R0 < 1 THAN GOTO M_LABEL(1)

        // R0 => 1 : run
        I_WR_REG(RTC_GPIO_OUT_REG, rtc_pin_bit, rtc_pin_bit, 1),    // Set PIN to HIGH
        M_BX(2),                                // GOTO M_LABEL(2)

        // R0 < 1 : run
        M_LABEL(1),                             // M_LABEL(1)
        I_WR_REG(RTC_GPIO_OUT_REG, rtc_pin_bit, rtc_pin_bit, 0),    // Set PIN to LOW

        M_LABEL(2),                             // M_LABEL(2)
        I_DELAY(60000),
        M_BX(2)                                 // GOTO M_LABEL(2)
    };

  // Run the program shifted backward by the number of variables
    size_t size = sizeof(ulp_prog) / sizeof(ulp_insn_t);
    ulp_process_macros_and_load(SLOW_PROG_ADDR, ulp_prog, &size);
    ulp_run(SLOW_PROG_ADDR);
}



void ulp_blink(uint32_t us, bool blink_enabled) {
  // Set ULP activation interval
    ulp_set_wakeup_period(0, us);

  // Slow memory initialization
    memset(RTC_SLOW_MEM, 0, 8192);

  // status initialization
    RTC_SLOW_MEM[SLOW_BLINK_EN] = (int)blink_enabled; // Enable/Disable LED Blink
    RTC_SLOW_MEM[SLOW_GPS_POWER] = 1;           // Enable GPS Switch


  // RTC PIN assignment
    const gpio_num_t rtc_pin = (gpio_num_t)LED_ESP;          // Ex.: GPIO_NUM_12
    const int rtc_pin_bit = RTC_GPIO_channels[LED_ESP] + 14;
  

  // GPIO initialization (set to output and initial value is 0)
    rtc_gpio_init(rtc_pin);
    rtc_gpio_set_direction(rtc_pin, RTC_GPIO_MODE_OUTPUT_ONLY);
    rtc_gpio_set_level(rtc_pin, 0);

  // ULP Program
    const ulp_insn_t  ulp_prog[] = {
        I_MOVI(R3, SLOW_BLINK_EN),              // R3 = SLOW_BLINK_EN
        I_LD(R0, R3, 0),                        // R0 = RTC_SLOW_MEM[R3(SLOW_BLINK_EN)]
        M_BL(2, 1),                             // IF R0 < 1 THAN GOTO M_LABEL(2)

        M_LABEL(1),                                                 // M_LABEL(1)
        I_WR_REG(RTC_GPIO_OUT_REG, rtc_pin_bit, rtc_pin_bit, 1),    // Set PIN to HIGH
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),

        I_WR_REG(RTC_GPIO_OUT_REG, rtc_pin_bit, rtc_pin_bit, 0),    // Set PIN to LOW
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),
        I_DELAY(60000),

        M_BX(1),                                                    // GOTO M_LABEL(1)

        M_LABEL(2),                                                 // M_LABEL(2)
        I_END()
    };

  // Run the program shifted backward by the number of variables
    size_t size = sizeof(ulp_prog) / sizeof(ulp_insn_t);
    ulp_process_macros_and_load(SLOW_PROG_ADDR, ulp_prog, &size);
    ulp_run(SLOW_PROG_ADDR);
}
