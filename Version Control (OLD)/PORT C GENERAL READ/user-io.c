#include "user-io.h"

#include <avr/io.h>
#include <util/delay.h>

//#include "wiring_private.h"
//#include "Arduino.h"
//#include "Wire.h"
//#include "pins_arduino.h"

/* LED (digital pin 13) on port B */
#define PORTB_LED (1 << 5)

/* Button on digital pin 12 port B */
#define PORTB_BUTTON (1 << 4)
#define PORTB_BUTTON2 (1 << 3)
#define PORTB_BUTTON3 (1 << 2)
#define PORTB_BUTTON4 (1 << 1)
#define PORTB_BUTTON5 (1 << 0)

#define PORTD_BUTTON1 (1 << 7)
#define PORTD_BUTTON2 (1 << 6)

#define PORTC_POT1 (1 << 4) //POT test on A4

/* Buzzer (digital pin 2) on port D */
#define PORTD_BUZZER (1 << 2)

/* Button minimum hold time (ms) -- avoid counting bounces as presses */
#define BUTTON_HOLD_TIME_MS 20


/* Structure to track button presses */
struct button_info {
	uint8_t hold_time; /* Time the button was held down */
	uint8_t count; /* Number of times the button was pressed */
};


/* Static functions */
static void track_button(struct button_info* info);
static uint8_t get_tracked_presses(const struct button_info* info);


/* Initializes the LED/button interface. */
void init_led_button(void)
{
	/* Configure LED as output, buzzer as output, button as input */
	//DDRB = (DDRB | PORTB_LED) & (~PORTB_BUTTON) & (~PORTB_BUTTON2) & (~PORTB_BUTTON3) & (~PORTB_BUTTON4) & (~PORTB_BUTTON5);
	DDRB = 0xC0;
	DDRB |= ~_BV(DDB5);
	DDRD = (DDRD) & (~PORTD_BUTTON1) & (~PORTD_BUTTON2);

	DDRC = 0xC0;


	/* Enable pullup on buttons? */
	PORTB |= PORTB_BUTTON;
	PORTB |= PORTB_BUTTON2;
	PORTB |= PORTB_BUTTON3;
	PORTB |= PORTB_BUTTON4;
	PORTB |= PORTB_BUTTON5;

	PORTD |= PORTD_BUTTON1;
	PORTD |= PORTD_BUTTON2;

	PORTC |= PORTC_POT1;


}


/* Wait the specified amount of time for the button to be pressed. */
bool wait_for_button_timeout(uint16_t led_on_time_ms, uint16_t led_off_time_ms,
	uint16_t timeout_ms)
{
	const uint16_t led_cycle_time_ms = led_on_time_ms + led_off_time_ms;
	uint16_t led_cycle_pos = 1;
	struct button_info info = {0, 0};

	while (timeout_ms > 0) {
		if (led_cycle_pos == 1) {
			PORTB |= PORTB_LED;
		} else if (led_cycle_pos == led_on_time_ms) {
			PORTB &= ~PORTB_LED;
		} else if (led_cycle_pos == led_cycle_time_ms) {
			led_cycle_pos = 0;
		}

		track_button(&info);

		if (info.count) {
			break;
		}

		timeout_ms -= 1;
		led_cycle_pos += 1;
	}

	/* Will wait for the button to be released */
	uint8_t presses = get_tracked_presses(&info);
	PORTB &= ~PORTB_LED;

	return presses > 0;
}


/* Blink the LED and wait for the user to press the button. */
uint8_t count_button_presses(uint16_t led_on_time_ms,
	uint16_t led_off_time_ms)
{
	const uint16_t led_cycle_time_ms = led_on_time_ms + led_off_time_ms;
	uint16_t led_cycle_pos = 1;
	struct button_info info = {0, 0};
	uint16_t timeout_ms = 0;

	while ((info.count == 0) || (timeout_ms > 0)) {
		if (led_cycle_pos == 1) {
			PORTB |= PORTB_LED;
		} else if (led_cycle_pos == led_on_time_ms) {
			PORTB &= ~PORTB_LED;
		} else if (led_cycle_pos == led_cycle_time_ms) {
			led_cycle_pos = 0;
		}

		track_button(&info);

		if (info.hold_time) {
			timeout_ms = 500;
		}

		timeout_ms -= 1;
		led_cycle_pos += 1;
	}

	PORTB &= ~PORTB_LED;

	/* Will wait for the button to be released */
	return get_tracked_presses(&info);
}

/* Wait a fixed amount of time, blinking the LED */
uint8_t delay(uint16_t led_on_time_ms, uint16_t led_off_time_ms,
	uint16_t delay_ms)
{
	uint16_t led_cycle_time_ms = led_on_time_ms + led_off_time_ms;
	uint16_t led_cycle_pos = 1;
	struct button_info info = {0, 0};
	uint16_t remaining = delay_ms;

	while (remaining > 0) {
		if (led_on_time_ms != 0) {
			if (led_cycle_pos == 1) {
				PORTB |= PORTB_LED;
			} else if (led_cycle_pos == led_on_time_ms) {
				PORTB &= ~PORTB_LED;
			} else if (led_cycle_pos == led_cycle_time_ms) {
				led_cycle_pos = 0;
			}
		}

		track_button(&info);

		remaining -= 1;
		led_cycle_pos += 1;
	}

	PORTB &= ~PORTB_LED;

	if (delay_ms <= BUTTON_HOLD_TIME_MS) {
		/* The wait delay is lower than the minimum hold time, so
		   get_tracked_presses will not return a correct number of press times.
		   Instead, we just return 1 if the button was held at all. */

		return (info.hold_time > 0) ? 1 : 0;
	}

	/* Will wait for the button to be released */
	return get_tracked_presses(&info);
}


/* Emit a brief beep the buzzer. */
void beep(void)
{
	PORTD |= PORTD_BUZZER;
	_delay_ms(1);
	PORTD &= ~PORTD_BUZZER;
}


/*
 * Track the button presses during roughly 1 ms.
 * The info struct must be initialized to all zeros before calling this
 * function.
 */
void track_button(struct button_info* info)
{
	bool button_held = ((PINB & PORTB_BUTTON) == 0);

	if (button_held) {
		/* The button is held; increment the hold time */
		info->hold_time += 1;

	} else {
		/* Check if the button was just released after being held for
		   a sufficient time */
		if (info->hold_time > BUTTON_HOLD_TIME_MS) {
			info->count += 1;
		}

		info->hold_time = 0;
	}

	_delay_ms(1);
}


/*
 * Count the button presses after a tracking operation.
 */
uint8_t get_tracked_presses(const struct button_info* info)
{
	uint8_t count = info->count;

	/* Wait for the button to be released */
	while ((PINB & PORTB_BUTTON) == 0) {
		/* Nothing */
	}

	/* Count the last button press (if the button was still held the last time
	   track_button was called */
	if (info->hold_time > BUTTON_HOLD_TIME_MS) {
		count += 1;
	}

	return count;
}

