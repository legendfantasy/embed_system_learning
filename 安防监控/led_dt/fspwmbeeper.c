#include "linux/perf_event.h"
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/cdev.h>

#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>

#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>

#include <linux/of.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/sched.h>  

#include <linux/clk.h>
#include <linux/pinctrl/consumer.h>

#include "fspwm.h"

#define FLAGS 1 
#define DEBUG_PRINTK(msg, DEBUG_FLAG) \
    do{ \
        if(!!DEBUG_FLAG) { \
            printk("---->%s--->%d\n",__func__,__LINE__);\
            printk(msg);\
		}\
    }while(0)


#define MY_DEV_CNT 1
//#define MANUAL
#ifdef MANUAL
	int major = 250;
	int minor = 0;
#endif
#define DEV_NAME "fs4412_pwm"
//register offset
#define GPD0CON       0x114000a0  

#define TCFG0         0x0000                 
#define TCFG1         0x0004                              
#define TCON          0x0008               
#define TCNTB0        0x000C            
#define TCMPB0        0x0010


// 设计一个全局对象,方便操作
typedef struct fsadc_dev
{
    dev_t devno;
    struct cdev pwmdev;
    struct class *cls;
    struct device *dev;
    
    struct resource *res;   //timer
    void *gpd0con_base;
    void *fs4412_pwm_base;
}PWM_DEV_T;

typedef struct beep_desc
{
	int beep;    //2 3 4 5
	int beep_state;  //0 or 1
	int tcnt;  //占空比
	int tcmp;  //调节占空比
}beep_desc_t;

#define mytype 'f'
#define BEEP_ON 	_IOW(mytype,0,beep_desc_t)
#define BEEP_OFF 	_IOW(mytype,1,beep_desc_t)
#define BEEP_FREQ 	_IOW(mytype,2,beep_desc_t)
void fs4412_beep_init(struct platform_device *pdev)
{
	DEBUG_PRINTK("fs4412_beep_init", FLAGS);
    PWM_DEV_T *samsung_pwm = platform_get_drvdata(pdev); 
    //GPD0_0 配置为PWM输出
    writel((readl(samsung_pwm->gpd0con_base) & ~(0xf<<0)) | (0x2<<0), samsung_pwm->gpd0con_base); // GPD0_0 : TOUT_0 
    //TCFG0 一级分频
    writel((readl(samsung_pwm->fs4412_pwm_base + TCFG0) & ~(0xff<<0)) | (0Xf9 <<0), samsung_pwm->fs4412_pwm_base + TCFG0); //设置默认值0XF9 = 249+1 250分频
    //TCFG1 二级分频
    writel((readl(samsung_pwm->fs4412_pwm_base + TCFG1) & ~(0xf<<0)) | (0x2<<0), samsung_pwm->fs4412_pwm_base + TCFG1 );  //4分频 	
    //调整后的PWM波形 100000 / (tcnt)100 = 1000hz （20 - 20000）hz 周期1ms //设置频率 -声音的高低
	//tcnt决定了周期  tcnt 取值范围：
    writel (100, samsung_pwm->fs4412_pwm_base + TCNTB0  );  //计数值 100次
    //tcmp设置占空比  50 / 100 = 50%   综述：占空比 = tcmp / tcnt;  //决定声音的大小
    writel (80, samsung_pwm->fs4412_pwm_base + TCMPB0 );  //比较值 50次 
    
	writel (readl(samsung_pwm->fs4412_pwm_base + TCON ) | (0x1 <<3), samsung_pwm->fs4412_pwm_base + TCON ); //设置自动重装载
	writel (readl(samsung_pwm->fs4412_pwm_base + TCON ) | (0x1 <<1), samsung_pwm->fs4412_pwm_base + TCON );  //设置手动重装
	writel (readl(samsung_pwm->fs4412_pwm_base + TCON ) & (~(1 << 1)), samsung_pwm->fs4412_pwm_base + TCON );//清除手动记载
}

static int fspwm_open(struct inode *pnode, struct file *pfile)
{
    DEBUG_PRINTK("fspwm_open", FLAGS);
	PWM_DEV_T *samsung_pwm = container_of(pnode->i_cdev, PWM_DEV_T, pwmdev);
	pfile->private_data = samsung_pwm;
    return 0;
}

static int fspwm_release(struct inode *inode, struct file *filp)
{
	DEBUG_PRINTK("fspwm_release", FLAGS);
	return 0;
}

static long fspwm_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	PWM_DEV_T *samsung_pwm = filp->private_data;
    beep_desc_t *beeper = (beep_desc_t *)arg;
	if (_IOC_TYPE(cmd) != mytype)
		return -ENOTTY;
    switch(cmd)  
    {  
        case BEEP_ON:  
            writel (readl(samsung_pwm->fs4412_pwm_base + TCON ) | (0x1 <<0), samsung_pwm->fs4412_pwm_base + TCON ); 
            break;  
        case BEEP_OFF:  
            writel (readl(samsung_pwm->fs4412_pwm_base + TCON ) | (~(1 << 0)), samsung_pwm->fs4412_pwm_base + TCON ); 
            break;  
        case BEEP_FREQ:  
            //tcnt决定了周期  tcnt 取值范围：
            writel (beeper->tcnt, samsung_pwm->fs4412_pwm_base + TCNTB0  );  //计数值 100次
            //tcmp设置占空比  50 / 100 = 50%   综述：占空比 = tcmp / tcnt;  //决定声音的大小
            writel (beeper->tcmp, samsung_pwm->fs4412_pwm_base + TCMPB0 );  //比较值 50次 
            break;  
        default :  
            return -EINVAL;  
    }  

	return 0;
}

static struct file_operations fs4412_pwm_ops = {
	.owner = THIS_MODULE,
	.open = fspwm_open,
	.release = fspwm_release,
	.unlocked_ioctl = fspwm_ioctl,
};
static int fs4412_pwm_probe(struct platform_device *pdev)  
{  
    DEBUG_PRINTK("fs4412_pwm_probe", FLAGS);
    printk("match success!\n"); 
    int ret;  
//---------------------申请自定义设备的内存空间------------------------------------
    PWM_DEV_T *samsung_pwm = kzalloc(sizeof(PWM_DEV_T), GFP_KERNEL);  //给设备对象分配内存
	if(NULL == samsung_pwm)
	{
		printk("kzalloc errorn\n");
		return -ENOMEM;
	}	
    platform_set_drvdata(pdev, samsung_pwm);
    //将samsung_adc的地址与某个平台设备（pdev）关联起来。这样，在后续的设备驱动操作中，可以通过pdev找到这个设备对象
//---------------------申请设备号--------------------------------------
#ifdef MANUAL  //手动指定设备号
	samsung_pwm->devno = MKDEV(major, minor);
	ret = register_chrdev_region(samsung_pwm->devno, MY_DEV_CNT, "pwm");
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
	ret = alloc_chrdev_region(&samsung_pwm->devno, 0, MY_DEV_CNT, "pwm");
	if(ret)
	{
		printk(KERN_ERR "auto get devno failed\n");
		ret = -ENODEV;  //ENODEV表示设备号获取失败
        goto devno_err;
	}
    printk(KERN_INFO " alloc_chrdev_region  success ok \n");
    printk(KERN_INFO "major = %d, minor = %d \n", MAJOR(samsung_pwm->devno), MINOR(samsung_pwm->devno));
#endif

//--------------------自动创建设备节点----------------------------------------
    samsung_pwm->cls = class_create(THIS_MODULE, "fspwm");
	if(IS_ERR(samsung_pwm->cls))
	{
		printk(KERN_ERR "class create failed\n");
        ret = PTR_ERR(samsung_pwm->cls);
        goto cls_err;
	}
    else
    {
        printk(KERN_INFO "class create success\n");
    }
	samsung_pwm->dev = device_create(samsung_pwm->cls, NULL, samsung_pwm->devno, NULL, "fspwm"); 
	if(IS_ERR(samsung_pwm->dev))  //判断是否创建成功
	{
		printk(KERN_ERR "device_create failed\n");
        ret = PTR_ERR(samsung_pwm->dev);
        goto dev_err;
	}
    else 
    {
        printk(KERN_INFO "device_create success\n");
    }
//---------------------注册字符设备驱动------------------------------------
	cdev_init(&samsung_pwm->pwmdev, &fs4412_pwm_ops);
	samsung_pwm-> pwmdev.owner = THIS_MODULE;
	ret = cdev_add(&samsung_pwm->pwmdev, samsung_pwm->devno, MY_DEV_CNT);
    if(ret)
    {
        goto add_err;
    }
//---------------------   获取资源    ------------------------------------
    samsung_pwm->res = platform_get_resource(pdev, IORESOURCE_MEM, 0); 
    printk(KERN_INFO "res = %p\n", samsung_pwm->res->start);
    if(NULL == samsung_pwm->res)
	{
		printk(KERN_ERR "platform_get_resource mem error\n");
		return -ENOENT;  //ENOENT表示没有找到资源
		goto mem_err;
	}
    //0x139D0000
    samsung_pwm->fs4412_pwm_base = ioremap(samsung_pwm->res->start, resource_size(samsung_pwm->res));   
	if(samsung_pwm->fs4412_pwm_base == NULL)
    {
		printk(KERN_ERR "ioremap error\n");
		ret = -EBUSY;  //EBUSY表示内存映射失败
		goto map_err;
	}
	else
	{
		printk("TCFG0 ioremap ok\n");
	}
    samsung_pwm->gpd0con_base = ioremap(GPD0CON, 4);
    if(samsung_pwm->gpd0con_base == NULL)
    {
        printk("GPD0CON ioremap error\n");
        ret = -EBUSY;  //EBUSY表示内存映射失败
        goto map_err;
    }
    else
    {
        printk("GPD0CON ioremap ok\n");
    }
    fs4412_beep_init(pdev);

    return 0;  



map_err:
mem_err:
    cdev_del(&samsung_pwm->pwmdev);
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

static int fs4412_pwm_remove(struct platform_device *pdev)  
{  

    DEBUG_PRINTK("fs4412_pwm_remove", FLAGS);
    PWM_DEV_T *samsung_pwm = platform_get_drvdata(pdev); 

    iounmap(samsung_pwm->fs4412_pwm_base);
    cdev_del(&samsung_pwm->pwmdev);
    unregister_chrdev_region(samsung_pwm->devno, MY_DEV_CNT);
    kfree(samsung_pwm);
    return 0;  
}  
  
static struct of_device_id fspwm_match_table[]=  
{  
    [0] = {.compatible = "fs4412,fspwm"},  
};  

static struct platform_driver fs4412_pwm_driver=  
{  
    .probe = fs4412_pwm_probe,  
    .remove = fs4412_pwm_remove,  
    .driver =                         
    {  
        .name = "fspwm",                   
        .owner = THIS_MODULE,             
        .of_match_table = fspwm_match_table,   
    },  
};  
  
static int fs4412_pwm_init(void)  
{  
    DEBUG_PRINTK("fs4412_pwm_init", FLAGS);
    return platform_driver_register(&fs4412_pwm_driver);  
}  
static void fs4412_pwm_exit(void)  
{  
    DEBUG_PRINTK("fs4412_pwm_exit", FLAGS);
    return platform_driver_unregister(&fs4412_pwm_driver);
}  

MODULE_LICENSE("GPL");  
module_init(fs4412_pwm_init);  
module_exit(fs4412_pwm_exit);  
