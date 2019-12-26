#ifndef __led_ctrl_h____
#define __led_ctrl_h____

void led_init();

void led_trig_green(int insta);
void led_trig_red(int insta);

int led_get_green();
int led_get_red();
#endif







