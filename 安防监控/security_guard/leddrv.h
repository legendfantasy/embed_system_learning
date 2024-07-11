#ifndef LED_DRIVER_H
#define LED_DRIVER_H
 
#define LED_DEV_MAGIC 'g'
 
typedef struct led_desc
{
 	int led_num;       // 2 3 4 5
 	int led_state;     // 0 or 1
}led_desc_t;
 
#define FS_LED_ON _IOW(LED_DEV_MAGIC, 1, led_desc_t)
#define FS_LED_OFF _IOW(LED_DEV_MAGIC, 0, led_desc_t)
 
 
#endif
