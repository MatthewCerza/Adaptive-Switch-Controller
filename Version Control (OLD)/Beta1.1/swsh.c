/*
 * Ice-cream Hotdogs
 */

#include <util/delay.h>
#include <avr/io.h>

#include "automation-utils.h"
#include "user-io.h"

#define PORTB_BUTTON (1 << 4) //A Button on Pin 12
#define PORTB_BUTTON2 (1 << 3) //B Button on Pin 11
#define PORTB_BUTTON3 (1 << 2) // D-pad up on Pin 10
#define PORTB_BUTTON4 (1 << 1) // D-pad down on Pin 9
#define PORTB_BUTTON5 (1 << 0) //D-pad left on Pin 8

#define PORTD_BUTTON1 (1 << 7) //D-pad right on Pin 7
#define PORTD_BUTTON2 (1 << 6) //X Button on Pin 6

/* Static functions */
static void temporary_control(void);
static void repeat_press_a(void);
static bool check_button_press(void);


int main(void)
{
	init_automation();
	init_led_button();

	/* Set the virtual controller as controller 1 */
	switch_controller(REAL_TO_VIRT);

	for (;;) {
		/* Set the LEDs, and make sure automation is paused while in the
		   menu */
		set_leds(BOTH_LEDS);

		bool A_button_held = ((PINB & PORTB_BUTTON) == 0);
		if (A_button_held){
			send_update(BT_A, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL);
			send_update(BT_NONE, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL);
		}
		_delay_ms(10);

		bool UP_button_held = ((PINB & PORTB_BUTTON3) == 0);
		if (UP_button_held){
			send_update(BT_NONE, DP_TOP, S_NEUTRAL, S_NEUTRAL);
			send_update(BT_NONE, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL);
		}
		_delay_ms(10);

		bool DOWN_button_held = ((PINB & PORTB_BUTTON4) == 0);
		if (DOWN_button_held){
			send_update(BT_NONE, DP_BOTTOM, S_NEUTRAL, S_NEUTRAL);
			send_update(BT_NONE, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL);
		}
		_delay_ms(10);

		bool B_button_held = ((PINB & PORTB_BUTTON2) == 0);
		if (B_button_held){
			send_update(BT_B, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL);
			send_update(BT_NONE, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL);
		}

		_delay_ms(10); //HELP ME
		bool LEFT_button_held = ((PINB & PORTB_BUTTON5) == 0);
		if (LEFT_button_held){
			send_update(BT_NONE, DP_LEFT, S_NEUTRAL, S_NEUTRAL);
			send_update(BT_NONE, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL);
		}

		_delay_ms(10); //PORT D WORKS YAS
		bool RIGHT_button_held = ((PIND & PORTD_BUTTON1) == 0);
		if (RIGHT_button_held){
			send_update(BT_NONE, DP_RIGHT, S_NEUTRAL, S_NEUTRAL);
			send_update(BT_NONE, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL);
		}

		_delay_ms(10);
		bool X_button_held = ((PIND & PORTD_BUTTON2) == 0);
		if (X_button_held){
			send_update(BT_X, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL);
			send_update(BT_NONE, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL);
		}



		send_update(BT_NONE, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL);
		_delay_ms(10);



  		//int value2 = (PINB |= ~_BV(DDB5));
		/*if(PINB & 00100000){
    		send_update(BT_A, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL);
			send_update(BT_NONE, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL); //press A once per cycle
  		} */
		/* Feature selection menu
		uint8_t count = count_button_presses(100, 900);

		for (uint8_t i = 0 ; i < count ; i += 1) {
			beep();
			_delay_ms(100);
		}
		*/
		/*switch (count) {
			case 1:
				temporary_control();
			break;

			case 2:
				repeat_press_a();
			break;

			case 3:
				max_raid_menu();
			break;

			case 4:
				auto_breeding();
			break;

			case 5:
				release_full_boxes();
			break;

			case 6:
				scan_boxes();
			break;

			default:

				delay(100, 200, 1500);
			break;
		}
		*/
	}
}


/*
 * Temporary gives back control to the user by performing controller switch.
 */
void temporary_control(void)
{
	set_leds(NO_LEDS);

	/* Allow the user to connect their controller back as controller 1 */
	switch_controller(VIRT_TO_REAL);

	/* Wait for the user to press the button (should be on the Switch main menu) */
	count_button_presses(100, 100);

	/* Set the virtual controller as controller 1 */
	switch_controller(REAL_TO_VIRT);
}


/*
 * Press A repetitively until the button is pressed.
 */
static void repeat_press_a(void)
{
	uint8_t count = 0;
	while (delay(0, 0, 50) == 0) {
		switch (count % 4) {
			case 0:
				set_leds(NO_LEDS);
			break;
			case 1:
				set_leds(TX_LED);
			break;
			case 2:
				set_leds(BOTH_LEDS);
			break;
			case 3:
				set_leds(RX_LED);
			break;
		}

		send_update(BT_A, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL);
		send_update(BT_NONE, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL);

		count += 1;
	}

	set_leds(NO_LEDS);
	send_update(BT_NONE, DP_NEUTRAL, S_NEUTRAL, S_NEUTRAL);

	_delay_ms(200);
}

/*
 * Checks if the button is pressed for a short period of time.
 */
bool check_button_press(void)
{
	return delay(0, 0, 20) != 0;
}
