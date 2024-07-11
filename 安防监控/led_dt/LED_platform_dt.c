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
#include <asm/uaccess.h>
#include <asm/atomic.h>

#include "leddrv.h"

#define MY_DEV_CNT 1
//#define MANUAL
#define USE_CGI_METHOD
#ifdef MANUAL
	int major = 11;
	int minor = 0;
#endif

struct myled_dev
{
	struct cdev mydev;	
	struct class *pcls;
	struct device *pdev;

	dev_t devno;
	atomic_t openflag;      //设备只打开一次
	unsigned int led2gpio;
	unsigned int led3gpio;
	unsigned int led4gpio;
	unsigned int led5gpio;	
};

struct myled_dev *pgmydev = NULL;  //定义一个*全局指针*用于操作设备

int myled_open(struct inode *pnode, struct file *pfile)
{
	pfile->private_data = (void *) (container_of(pnode->i_cdev, struct myled_dev, mydev));
	/* 控制设备只打开一次 */
	if(atomic_dec_and_test(&pgmydev->openflag))
	{
		printk("The device is opened successed\n");
		return 0;
	}
	else
	{
		atomic_inc(&pgmydev->openflag);
		printk("The device is opened already\n");
		return -1;
	}
}
 
int myled_close(struct inode *pnode, struct file *pfile)
{
	atomic_set(&pgmydev->openflag, 1);
	printk("mydev is closed\n");
	return 0;
}
 
void led_on(struct myled_dev *pmydev, int ledno)
{
	switch(ledno)
	{
		case 2:
			gpio_set_value(pmydev->led2gpio, 1);  //设置gpio为高电平
			break;
		case 3:
			gpio_set_value(pmydev->led3gpio, 1);
			break;
		case 4:
			gpio_set_value(pmydev->led4gpio, 1);
			break;
		case 5:
			gpio_set_value(pmydev->led5gpio, 1);
			break;
	}
}
 
void led_off(struct myled_dev *pmydev,int ledno)
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
 
 
long myled_ioctl(struct file *pfile, unsigned int cmd, unsigned long arg)
{

#ifndef USE_CGI_METHOD
	struct myled_dev *pmydev = (struct myled_dev *)pfile->private_data;
	if(arg < 2 || arg > 5)
	{
		return -1;
	}
	switch(cmd)
	{
		case FS_LED_ON:
			led_on(pmydev, arg);
			break;
		case FS_LED_OFF:
			led_off(pmydev, arg);
			break;
		default:
			return -1;
	}
	return 0;
#else
	struct myled_dev *pmydev = (struct myled_dev *)pfile->private_data;
	int no;
	led_desc_t *led = (led_desc_t *)arg;
	no = led->led_num;
	switch(cmd)
	{
		case MY_LED_ON:
			led_on(pmydev, no);
			break;
		case MY_LED_OFF:
			led_off(pmydev, no);
			break;
		default:
			return -1;
	}
	return 0;
#endif
}
 
struct file_operations myops = {
	.owner = THIS_MODULE,
	.open = myled_open,
	.release = myled_close,
	.unlocked_ioctl = myled_ioctl,
};

void request_leds_gpio(struct myled_dev *pmydev, struct device_node *pnode)
{
	pmydev->led2gpio = of_get_named_gpio(pnode,"led2-gpio",0);  
	gpio_request(pmydev->led2gpio,"led2");
	
	pmydev->led3gpio = of_get_named_gpio(pnode,"led3-gpio",0);
	gpio_request(pmydev->led3gpio,"led3");
	
	pmydev->led4gpio = of_get_named_gpio(pnode,"led4-gpio",0);
	gpio_request(pmydev->led4gpio,"led4");
	
	pmydev->led5gpio = of_get_named_gpio(pnode,"led5-gpio",0);
	gpio_request(pmydev->led5gpio,"led5");
}
void set_leds_gpio_output(struct myled_dev *pmydev)
{
	// 第一个是GPIO引脚的编号，value是要设置的初始输出值
	gpio_direction_output(pmydev->led2gpio, 0);
	gpio_direction_output(pmydev->led3gpio, 0);
	gpio_direction_output(pmydev->led4gpio, 0);
	gpio_direction_output(pmydev->led5gpio, 0);
}
void free_leds_gpio(struct myled_dev *pmydev)
{
	gpio_free(pmydev->led2gpio);
	gpio_free(pmydev->led3gpio);
	gpio_free(pmydev->led4gpio);
	gpio_free(pmydev->led5gpio);
}

int myled_probe(struct platform_device *p_pltdev)
{
	int ret = 0;
	struct device_node *pnode = NULL;
//---------------------申请自定义设备的内存空间------------------------------------	
	pgmydev = (struct myled_dev *)kmalloc(sizeof(struct myled_dev), GFP_KERNEL);
	if(NULL == pgmydev)
	{
		printk("kmalloc failed\n");
		return -1;
	}
	memset(pgmydev, 0, sizeof(struct myled_dev));
//---------------------申请设备号--------------------------------------
#ifdef MANUAL  //手动指定设备号
	pgmydev->devno = MKDEV(major, minor);
	ret = register_chrdev_region(pgmydev->devno, MY_DEV_CNT, "fsled");
	if(ret)
	{
		printk("get devno failed\n");
		return -1;
	}
#else  //自动获取设备号
	ret = alloc_chrdev_region(&pgmydev->devno, 0, MY_DEV_CNT, "fsled");
	if(ret)
	{
		printk("auto get devno failed\n");
		return -1;
	}
	printk(KERN_INFO " alloc_chrdev_region  success ok \n");
	printk(KERN_INFO "major = %d, minor = %d \n", MAJOR(pgmydev->devno), MINOR(pgmydev->devno));
	//pgmydev->devno = MKDEV(MAJOR(pgmydev->devno), MINOR(pgmydev->devno));
#endif
//--------------------自动创建设备节点----------------------------------------
	pgmydev->pcls = class_create(THIS_MODULE, "fsled");
	if(IS_ERR(pgmydev->pcls))
	{
		printk("class create failed\n");
		cdev_del(&pgmydev->mydev);
		unregister_chrdev_region(pgmydev->devno, MY_DEV_CNT);
		return -1;
	}
	pgmydev->pdev = device_create(pgmydev->pcls, NULL, pgmydev->devno, NULL, "fsled"); 
	if(NULL == pgmydev->pdev)  //判断是否创建成功
	{
		printk(KERN_INFO "device_create failed\n");
		class_destroy(pgmydev->pcls);
		cdev_del(&pgmydev->mydev);
		unregister_chrdev_region(pgmydev->devno, MY_DEV_CNT);
		return -1;
	}
//---------------------注册字符设备驱动------------------------------------
	cdev_init(&pgmydev->mydev, &myops);
	pgmydev->mydev.owner = THIS_MODULE;
	cdev_add(&pgmydev->mydev, pgmydev->devno, MY_DEV_CNT);
//---------------------配置硬件资源------------------------------------
	pnode = p_pltdev->dev.of_node;  // 获取设备树节点
	request_leds_gpio(pgmydev, pnode);
	set_leds_gpio_output(pgmydev);

	atomic_set(&pgmydev->openflag, 1);
	return 0;
}
 
int myled_remove(struct platform_device *p_pltdev)
{
	/*iounmap*/
	free_leds_gpio(pgmydev);
	cdev_del(&pgmydev->mydev);
	unregister_chrdev_region(pgmydev->devno, MY_DEV_CNT);
	kfree(pgmydev);
	pgmydev = NULL;
 
	return 0;
}

struct of_device_id leddrv_of_ids[] = 
{
	[0] = {.compatible = "fs4412,led2-5"},
	[1] = {.compatible = "origen4412,led6-9"},
	[2] = {},
};

struct platform_driver myled_driver = 
{
	.driver = {
		.name = "fs4412leds",
		.of_match_table = leddrv_of_ids,
	},
	.probe = myled_probe,
	.remove = myled_remove,
};
// 驱动程序的名称被设置为 “fs4412leds”，并且
//of_match_table 字段被设置为 myleddrv_of_ids，这意味着该驱动程序将用于与 myleddrv_of_ids 数组中定义的设备进行匹配
int __init myled_init(void)
{
	platform_driver_register(&myled_driver); 
	return 0;
}
 
void __exit myled_exit(void)
{
	platform_driver_unregister(&myled_driver);
}

 
MODULE_LICENSE("GPL");
module_init(myled_init);
module_exit(myled_exit);
