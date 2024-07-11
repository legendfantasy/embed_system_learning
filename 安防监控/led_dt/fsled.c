#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <linux/types.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>

#include "fsled.h"

#define FLAGS 1 
#define DEBUG_PRINTK(msg, DEBUG_FLAG) \
		do{ \
			if(!!DEBUG_FLAG) { \
				printk("---->%s--->%d\n",__func__,__LINE__);\
				printk(msg);\
			}\
		}while(0)

#define DEV_NUM 1
#define DEV_NAME "fsled"
//#define MANUAL
#ifdef MANUAL
	int major = 250;
	int minor = 0;
#endif

typedef struct led_dev
{
	//设备节点相关
	dev_t devno;
	struct cdev leddev;	
	struct class *pcls;
	struct device *pdev;
	//设备驱动操作相关
	atomic_t openflag;      //设备只打开一次
	//设备驱动资源相关
	unsigned int led2gpio;
	unsigned int led3gpio;
	unsigned int led4gpio;
	unsigned int led5gpio;	
}LED_DEV_T;

struct fsled_dev *samsung_led = NULL;  //定义一个*全局指针*用于操作设备

int fsled_open(struct inode *pnode, struct file *pfile)
{
	DEBUG_PRINTK("fsled_open", FLAGS);

	LED_DEV_T *samsung_led = container_of(pnode->i_cdev, LED_DEV_T, leddev);
	pfile->private_data = samsung_led;

	/* 控制设备只打开一次 */
	if(atomic_dec_and_test(&samsung_led->openflag))
	{
		printk("The device is opened successed\n");
		return 0;
	}
	else
	{
		atomic_inc(&samsung_led->openflag);
		printk("The device is opened already\n");
		return -1;
	}
}
 
int fsled_close(struct inode *pnode, struct file *pfile)
{
	LED_DEV_T *samsung_led = pfile->private_data
		
	atomic_set(&samsung_led->openflag, 1);
	printk("fsled is closed\n");
	return 0;
}
 
void led_on(LED_DEV_T *samsung_led, int ledno)
{
	switch(ledno)
	{
		case 2:
			gpio_set_value(samsung_led->led2gpio, 1);  //设置gpio为高电平
			break;
		case 3:
			gpio_set_value(samsung_led->led3gpio, 1);
			break;
		case 4:
			gpio_set_value(samsung_led->led4gpio, 1);
			break;
		case 5:
			gpio_set_value(samsung_led->led5gpio, 1);
			break;
	}
}
 
void led_off(LED_DEV_T *samsung_led, int ledno)
{
	switch(ledno)
	{
		case 2:
			gpio_set_value(pmydev->led2gpio, 0);
			break;
		case 3:
			gpio_set_value(pmydev->led3gpio, 0);
			break;
		case 4:
			gpio_set_value(pmydev->led4gpio, 0);
			break;
		case 5:
			gpio_set_value(pmydev->led5gpio, 0);
			break;
	}
}
 
 
long fsled_ioctl(struct file *pfile, unsigned int cmd, unsigned long arg)
{
	LED_DEV_T *samsung_led = (LED_DEV_T *)pfile->private_data;
	led_desc_t *led = (led_desc_t *)arg;
	if(arg < 2 || arg > 5)
	{
		printk(KERN_ERR "please assign a correct led number\n");
		return -1;
	}
	switch(cmd)
	{
		case FS_LED_ON:
			led_on(samsung_led, led->led_num);
			break;
		case FS_LED_OFF:
			led_off(samsung_led, led->led_num);
			break;
		default:
			return -1;
	}
	return 0;
}
 
struct file_operations myops = {
	.owner = THIS_MODULE,
	.open = fsled_open,
	.release = fsled_close,
	.unlocked_ioctl = fsled_ioctl,
};

void request_leds_gpio(LED_DEV_T *samsung_led, struct device_node *pnode)
{
	samsung_led->led2gpio = of_get_named_gpio(pnode, "led2-gpio", 0);  
	gpio_request(samsung_led->led2gpio, "led2");
	
	samsung_led->led3gpio = of_get_named_gpio(pnode, "led3-gpio", 0);
	gpio_request(samsung_led->led3gpio, "led3");
	
	samsung_led->led4gpio = of_get_named_gpio(pnode, "led4-gpio", 0);
	gpio_request(samsung_led->led4gpio, "led4");
	
	samsung_led->led5gpio = of_get_named_gpio(pnode, "led5-gpio", 0);
	gpio_request(samsung_led->led5gpio, "led5");
}
void set_leds_gpio_output(LED_DEV_T *samsung_led)
{
	// 第一个是GPIO引脚的编号，value是要设置的初始输出值
	gpio_direction_output(samsung_led->led2gpio, 0);
	gpio_direction_output(samsung_led->led3gpio, 0);
	gpio_direction_output(samsung_led->led4gpio, 0);
	gpio_direction_output(samsung_led->led5gpio, 0);
}
void free_leds_gpio(LED_DEV_T *samsung_led)
{
	gpio_free(samsung_led->led2gpio);
	gpio_free(samsung_led->led3gpio);
	gpio_free(samsung_led->led4gpio);
	gpio_free(samsung_led->led5gpio);
}

int fsled_probe(struct platform_device *p_pltdev)
{
	DEBUG_PRINTK("fsled_probe", FLAGS);
	printk("match success  !\n");
	int ret = 0;
	struct device_node *pnode = NULL;
//---------------------申请自定义设备的内存空间------------------------------------	
	LED_DEV_T *samsung_led = kzalloc(sizeof(LED_DEV_T),  GFP_KERNEL);
	if(samsung_led == NULL)
	{
		printk("kzalloc error\n");
		return -ENOMEM;
	}
	platform_set_drvdata(p_pltdev, samsung_led);
//---------------------申请设备号--------------------------------------
#ifdef MANUAL  //手动指定设备号
	samsung_led->devno = MKDEV(major, minor);
	ret = register_chrdev_region(samsung_led->devno, DEV_NUM, DEV_NAME);
	if(ret)
	{
		printk(KERN_ERR "register_chrdev error\n");
		ret = -ENODEV;  //ENODEV表示设备号获取失败
		goto devno_err;
	}
	else
    {
        printk(KERN_INFO "register_chrdev success\n");                                           
    }
#else  //自动获取设备号
	ret = alloc_chrdev_region(&samsung_led->devno, 0, DEV_NUM, DEV_NAME);
	if(ret)
	{
		printk(KERN_ERR "auto get devno failed\n");
		ret = -ENODEV;  //ENODEV表示设备号获取失败
        goto devno_err;

	}
	printk(KERN_INFO " alloc_chrdev_region  success ok \n");
	printk(KERN_INFO "major = %d, minor = %d \n", MAJOR(samsung_led->devno), MINOR(samsung_led->devno));
#endif
//--------------------自动创建设备节点----------------------------------------
	samsung_led->pcls = class_create(THIS_MODULE, DEV_NAME);
	if(IS_ERR(samsung_led->pcls))
	{
		printk(KERN_ERR "class create failed\n");
		ret = PTR_ERR(samsung_led->pcls);
		goto cls_err;
	}
	else
    {
        printk(KERN_INFO "class create success\n");
    }
	samsung_led->pdev = device_create(samsung_led->pcls, NULL, samsung_led->devno, NULL, DEV_NAME); 
	if(IS_ERR(samsung_led->pdev))  //判断是否创建成功
	{
		printk(KERN_ERR "device_create failed\n");
		ret = PTR_ERR(samsung_led->pdev);
		goto dev_err;
	}
	else 
    {
        printk(KERN_INFO "device_create success\n");
    }
//---------------------注册字符设备驱动------------------------------------
	cdev_init(&samsung_led->leddev, &myops);
	samsung_led->leddev.owner = THIS_MODULE;
	ret = cdev_add(&samsung_led->leddev, samsung_led->devno, DEV_NUM);
	if(ret)
	{
		goto add_err;
	}
//---------------------配置硬件资源------------------------------------
	pnode = p_pltdev->dev.of_node;  // 获取设备树节点
	request_leds_gpio(samsung_led, pnode);  
	set_leds_gpio_output(samsung_led);
//---------------------其他操作------------------------------------------
	atomic_set(&samsung_led->openflag, 1);

	return 0;
	
add_err:
	device_destroy(samsung_pwm->cls, samsung_pwm->devno);
dev_err:
	class_destroy(samsung_pwm->cls);
cls_err:
	unregister_chrdev_region(samsung_pwm->devno, MY_DEV_CNT);
devno_err:
	kfree(samsung_pwm);
	return ret;
}
 
int fsled_remove(struct platform_device *p_pltdev)
{
	/*iounmap*/
	free_leds_gpio(samsung_led);
	cdev_del(&samsung_led->mydev);
	unregister_chrdev_region(samsung_led->devno, DEV_NUM);
	kfree(samsung_led);
	samsung_led = NULL;
 
	return 0;
}

struct of_device_id leddrv_of_ids[] = 
{
	[0] = {.compatible = "fs4412,led2-5"},
	[1] = {.compatible = "origen4412,led6-9"},
	[2] = {},
};

struct platform_driver fsled_driver = 
{
	.driver = {
		.name = "fs4412leds",
		.of_match_table = leddrv_of_ids,
	},
	.probe = fsled_probe,
	.remove = fsled_remove,
};

int __init fsled_init(void)
{
	platform_driver_register(&fsled_driver);   //注册platform_driver
	return 0;
}
 
void __exit fsled_exit(void)
{
	platform_driver_unregister(&fsled_driver);
}

 
MODULE_LICENSE("GPL");
module_init(fsled_init);
module_exit(fsled_exit);
