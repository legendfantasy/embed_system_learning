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
#define DEV_NAME "fs4412_adc"
//register offset
#define  ADCCON         0x0000  
#define  ADCDLY         0x0008  
#define  ADCDAT         0x000C  
#define  CLRINTADC      0x0018  
#define  ADCMUX         0x001C

// 设计一个全局对象,方便操作
typedef struct fsadc_dev
{
    dev_t devno;
    struct cdev adcdev;
    struct class *cls;
    struct device *dev;

    wait_queue_head_t wq;  //等待队列

    int have_data;  //标志ADC是否完成转换
    int fs4412_adc;  //存储ADC转换后的数据
    struct resource *res1;
    struct resource *res2;  
    unsigned int irq;
    void *fs4412_adc_base;  //vir_addr(0x1260C 0000)

}ADC_DEV_T;


static irqreturn_t fs4412_adc_handler(int irqno, void *dev)  
{  
/*
函数的返回类型是irqreturn_t，这是一个内核定义的类型，用于指示中断是否被成功处理。
函数接受两个参数：irq（中断号）和dev_id（设备标识符）。
*/
    ADC_DEV_T *samsung_adc = (ADC_DEV_T *)dev;  
    samsung_adc->have_data = 1;   //唤醒条件
    /*清中断*/  
    writel(0x12,  samsung_adc->fs4412_adc_base + CLRINTADC);   //写任意值到寄存器清中断   
    wake_up_interruptible(&samsung_adc->wq);  // 唤醒等待队列
    return IRQ_HANDLED;  
}  
static ssize_t fs4412_adc_read(struct file *filep, char __user *buf, size_t len, loff_t *pos)  
{  
    ADC_DEV_T *samsung_adc = filep->private_data;  

    writel(0x3, samsung_adc->fs4412_adc_base + ADCMUX);    //0011 对应AIN3
    writel(1<<0 | 1<<14 | 0X1<<16 | 0XFF<<6 ,samsung_adc->fs4412_adc_base + ADCCON);  
    
    //waite_event_interruptible会阻塞等待中断唤醒
    wait_event_interruptible(samsung_adc->wq, samsung_adc->have_data == 1); 
    //等待中断唤醒,中断函数中唤醒等待队列
    /*read data & 0xfff,否则数据出错 */  
    samsung_adc->fs4412_adc = readl(samsung_adc->fs4412_adc_base + ADCDAT) & 0xfff;  

   //驱动层不进行运算，直接将数据拷贝给应用层
    if(copy_to_user(buf, &(samsung_adc->fs4412_adc), sizeof(int))) 
        return -EFAULT;  
    samsung_adc->have_data = 0;  
    return len;  
}  

static int fs4412_adc_open (struct inode *inod, struct file *filep)  
{  
  	DEBUG_PRINTK("fs4412_adc_open", FLAGS);
    ADC_DEV_T *samsung_adc = container_of(inod->i_cdev, ADC_DEV_T, adcdev);
	filep->private_data = samsung_adc;
    return 0;  
}  

static  int fs4412_adc_release(struct inode *inode, struct file *filep)  
{  
    DEBUG_PRINTK("fs4412_adc_release", FLAGS);
    ADC_DEV_T *samsung_adc = filep->private_data; 
    return 0;  
}  

static struct file_operations  fs4412_adc_ops ={  
    .open    = fs4412_adc_open,  
    .release = fs4412_adc_release,  
    .read    = fs4412_adc_read,  
};  
  
  
static int fs4412_adc_probe(struct platform_device *pdev)  
{  
    DEBUG_PRINTK("fs4412_adc_probe", FLAGS);
    printk("match 0k \n"); 
    int ret;  
//---------------------申请自定义设备的内存空间------------------------------------
    ADC_DEV_T *samsung_adc = kzalloc(sizeof(ADC_DEV_T), GFP_KERNEL);  //给设备对象分配内存
	if(NULL == samsung_adc)
	{
		printk("kzalloc errorn\n");
		return -ENOMEM;
	}	
    platform_set_drvdata(pdev, samsung_adc);
    //将samsung_adc的地址与某个平台设备（pdev）关联起来。这样，在后续的设备驱动操作中，可以通过pdev找到这个设备对象
//---------------------申请设备号--------------------------------------
#ifdef MANUAL  //手动指定设备号
	samsung_adc->devno = MKDEV(major, minor);
	ret = register_chrdev_region(samsung_adc->devno, MY_DEV_CNT, "adc");
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
	ret = alloc_chrdev_region(&samsung_adc->devno, 0, MY_DEV_CNT, "adc");
	if(ret)
	{
		printk(KERN_ERR "auto get devno failed\n");
		ret = -ENODEV;  //ENODEV表示设备号获取失败
        goto devno_err;
	}
    printk(KERN_INFO " alloc_chrdev_region  success ok \n");
    printk(KERN_INFO "major = %d, minor = %d \n", MAJOR(samsung_adc->devno), MINOR(samsung_adc->devno));
#endif

//--------------------自动创建设备节点----------------------------------------
	
    samsung_adc->cls = class_create(THIS_MODULE, "myadc");
	if(IS_ERR(samsung_adc->cls))
	{
		printk(KERN_ERR "class create failed\n");
        ret = PTR_ERR(samsung_adc->cls);
        goto cls_err;
	}
    else 
    {
        printk(KERN_INFO "class create success\n");
    }
	samsung_adc->dev = device_create(samsung_adc->cls, NULL, samsung_adc->devno, NULL, "myadc"); 
	if(IS_ERR(samsung_adc->dev))  //判断是否创建成功
	{
		printk(KERN_ERR "device_create failed\n");
        ret = PTR_ERR(samsung_adc->dev);
        goto dev_err;
	}
    else 
    {
        printk(KERN_INFO "device_create success\n");
    }
//---------------------注册字符设备驱动------------------------------------
	cdev_init(&samsung_adc->adcdev, &fs4412_adc_ops);
	samsung_adc-> adcdev.owner = THIS_MODULE;
	ret = cdev_add(&samsung_adc->adcdev, samsung_adc->devno, MY_DEV_CNT);
    if(ret)
    {
        goto add_err;
    }
//---------------------   获取资源    ------------------------------------
    //获取中断号
    samsung_adc->res1 = platform_get_resource(pdev, IORESOURCE_IRQ, 0); 
    printk("res1->start :%d.\n", samsung_adc->res1->start);
    if(NULL == samsung_adc->res1)
	{
		printk(KERN_ERR "platform_get_resource irq error\n");
		return -ENOENT;  //ENOENT表示没有找到资源
		goto irq_err;
	}
    // samsung_adc->irq = platform_get_irq(pdev, 0);
    // printk(KERN_INFO "irq = %d\n", samsung_adc->irq);
    // if(samsung_adc->irq < 0)
    // {
    //     printk(KERN_ERR "platform_get_resource irq error\n");
	//  	return -ENOENT;  //ENOENT表示没有找到资源
	//  	goto irq_err;
    // }
    //获取内存地址 0x126C0000
    samsung_adc->res2 = platform_get_resource(pdev, IORESOURCE_MEM, 0); 
    if(NULL == samsung_adc->res2)
	{
		printk(KERN_ERR "platform_get_resource mem error\n");
		return -ENOENT;  //ENOENT表示没有找到资源
		goto mem_err;
	}
//---------------------   配置获得的资源    ------------------------------------
    //获取内存地址 物理：0x126C0000 - 0x12600020 -> 虚拟地址
    samsung_adc->fs4412_adc_base = ioremap(samsung_adc->res2->start, samsung_adc->res2->end - samsung_adc->res2->start);   
    //把真实的物理地址映射到内核虚拟地址，第1个参数是物理地址，第2个参数是映射的长度
    //映射得到的地址保存在fs4412_adc_base中
	if(samsung_adc->fs4412_adc_base == NULL)
    {
		printk(KERN_ERR "ioremap error\n");
		ret = -EBUSY;  //EBUSY表示内存映射失败
		goto map_err;
	}
	else
	{
		printk("ioremap ok\n");
	}
    ret = request_irq(samsung_adc->res1->start, fs4412_adc_handler, 0, "adc1", samsung_adc);   //注册中断
    //第1个参数是中断号，第2个参数是中断处理函数，第3个参数是中断触发方式，第4个参数是中断名字，第5个参数是中断处理函数的参数
    //IRQF_DISABLED 标志告诉内核，当中断处理程序执行时，应该禁用本地中断。这意味着在中断处理程序执行期间，其他中断不会被处理。
    //这有时是必要的，以确保中断处理程序可以安全地访问共享资源或执行一些需要原子性（即不可中断性）的操作。
    if(ret)
    {
        printk("request irq error\n");
        goto irq_r_err;
    }
//---------------------   其他    ------------------------------------ 
    init_waitqueue_head(&samsung_adc->wq);  
      
    return 0;  


irq_r_err:
    iounmap(samsung_adc->fs4412_adc_base);
map_err:
mem_err:
irq_err:
    cdev_del(&samsung_adc->adcdev);
add_err:
	device_destroy(samsung_adc->cls, samsung_adc->devno);
dev_err:
	class_destroy(samsung_adc->cls);
cls_err:
	unregister_chrdev_region(samsung_adc->devno, MY_DEV_CNT);
devno_err:
	kfree(samsung_adc);
	return ret;
}  

static int fs4412_adc_remove(struct platform_device *pdev)  
{  

    DEBUG_PRINTK("fs4412_adc_remove", FLAGS);
    ADC_DEV_T *samsung_adc = platform_get_drvdata(pdev); 
    
    writel((readl(samsung_adc + ADCCON) & ~(1 << 16)) | (1 << 2), samsung_adc + ADCCON);
    //清除 adccon 寄存器的第 16 位 ，并设置第 2 位
    free_irq(samsung_adc->res2->start, samsung_adc);  //释放requset_irq申请的资源  
    iounmap(samsung_adc->fs4412_adc_base);   //释放ioremap申请的资源
    cdev_del(&samsung_adc->adcdev);
    unregister_chrdev_region(samsung_adc->devno, MY_DEV_CNT);
    kfree(samsung_adc);
    return 0;  
}  
  
static struct of_device_id fsadc_match_table[]=  
{  
    [0] = {.compatible = "fs4412,fsadc"},  
    //这是数组的第一个（也是唯一一个）元素。它是一个 of_device_id 类型的结构体，其中 compatible 成员被初始化为字符串 "fs4412,fsadc"。
    //compatible 成员通常用于匹配设备树中的设备节点和驱动程序。在这个例子中，如果设备树中有一个设备节点的 compatible 属性是 "fs4412,fsadc"，那么与该设备节点相关的设备可能会被这个驱动程序处理。
};  

//MODULE_DEVICE_TABLE(of, fsadc_match_table);
//注册设备树匹配表。当内核启动时，它会查找并加载与硬件兼容的驱动。
//of: 这个参数表示我们正在为“Open Firmware”设备树注册匹配表。Open Firmware（OF）是设备树规范的一部分，用于描述硬件配置。
//fsadc_of_matches: 这是设备树匹配表的名称。通常，这个表是一个数组，包含多个条目，每个条目描述了一个硬件设备或子系统的特征。每个条目通常包含兼容字符串、数据等，以便内核可以确定该驱动是否与特定的硬件设备或子系统匹配。
static struct platform_driver fs4412_adc_driver=  
{  
    .probe = fs4412_adc_probe,  
    .remove = fs4412_adc_remove,  
    //probe和remove分别是设备驱动的初始化和卸载函数，它们在驱动与硬件设备进行匹配时被调用。
    //probe函数通常在驱动与设备进行绑定时调用，以初始化设备和设置相应的回调函数；
    //remove函数在设备被卸载或驱动被移除时调用，用于清理probe函数所做的设置。
    .driver =                         
    {  
        .name = "fsadc",                   
        .owner = THIS_MODULE,             
        .of_match_table = fsadc_match_table,   
        //* `.name = "fsadc"`：指定了驱动的名字，这个名字在驱动注册时会与设备树（Device Tree）中的设备进行匹配。
        //* `.owner = THIS_MODULE`：通常用于内核模块，表示这个驱动属于当前模块。`THIS_MODULE`是一个宏，代表当前正在加载的内核模块。
        //* `.of_match_table = of_match_ptr(fsadc_match_table)`：指定了一个匹配表，用于在设备树中进行设备匹配。
        //`fsadc_match_table`可能是一个在代码中定义的数组，包含了用于匹配设备和驱动的信息。`of_match_ptr`是一个宏，用于将匹配表转换为`of_device_id`指针类型。
    },  
};  
  
static int fs4412_adc_init(void)  
{  
    DEBUG_PRINTK("fs4412_adc_init", FLAGS);
    return platform_driver_register(&fs4412_adc_driver);  
}  
static void fs4412_adc_exit(void)  
{  
    DEBUG_PRINTK("fs4412_adc_exit", FLAGS);
    return platform_driver_unregister(&fs4412_adc_driver);
}  

MODULE_LICENSE("GPL");  
module_init(fs4412_adc_init);  
module_exit(fs4412_adc_exit);  
