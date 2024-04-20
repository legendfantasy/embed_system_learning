#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>  // register_leddev
#include <linux/device.h>  //device_create
#include <asm/uaccess.h>  //copy_to_user
#include <asm/io.h>  //ioremap
#include <linux/slab.h>  //kmalloc

//设计一个类型，描述一个设备的信息
struct led_desc
{
    unsigned int dev_major; //主设备号
    struct class *cls;  //类指针
    struct device *dev;  //创建设备文件
    void *reg_virt_base;  //虚拟地址
}

struct led_desc *led_dev; //声明对象，用于全局共享

static int kernel_val = 555;

ssize_t led_drv_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    prink("-----%s-----\n", __FUCNTION__);
    int ret;
    ret = copy_to_user(buf, &kernel_val, count);
    if(ret > 0)
    {
        printk("copy_to_user failed\n");
        return -EFAULT;
    }
    return 0;
}
ssize_t led_drv_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    int ret;
    int value;

    ret = copy_from_user(&value, buf, count);
    if(ret > 0)
    {
        printk("copy_from_user failed\n");
        return -EFAULT;
    }
      
    if(value)
    {
        writel(readl(led_dev->reg_virt_base + 4) | (1 << 7), led_dev->reg_virt_base + 4);
    }
    else
    {
        writel(readl(led_dev->reg_virt_base + 4) & ~(1 << 7), led_dev->reg_virt_base + 4);
    }
    return 0;
}
int led_drv_open(struct inode *inode, struct file *file)
{
    prink("-----%s-----\n", __FUCNTION__);
    return 0;
}
int led_drv_close(struct inode *inode, struct file *file)
{
    prink("-----%s-----\n", __FUCNTION__);
    return 0;
}
static struct my_fops = {
    .open = led_drv_open,
    .read = led_drv_read,
    .write = led_drv_write,
    .release = led_drv_close,
};

static int led_dev_init(void)
{
    //0, 实例化全局的设备对象--分配空间
    // GFP_KERNEL, 如果当前内存不够，会阻塞
    led_dev = kmalloc(sizeof(struct led_desc), GFP_KERNEL);  //分配空间 
    if(led_dev == NULL)
    {
        printk(KERN_ERR "kmalloc failed\n");
        return -ENOMEM;
    }

    //模块装载，一般都是申请设备资源
    
/***************************申请设备号*********************************/
    /*静态指定  
    led_dev->dev_major = 250; 
    int ret;
    ret = register_chardev(led_dev->dev_major, "led_dev_test", &my_fops);
    
    if (ret == 0)
    {
        printk("register ok\n");
    }
    else
    {
        printk("register failed\n");
        return -EINVAL;
    }
    */
    /*动态分配*/
    led_dev->dev_major = register_chrdev(0, "led_dev_test", &my_fops);
    if (led_dev -> dev_major < 0)
    {
        printk(KERN_ERR "register failed\n");
        ret = -ENODEV;
        goto err_0; 
    }
/**********************创建设备节点***************************************/
    led_dev->cls = class_create(THIS_MODULE, "led_dev_test");
    if(IS_ERR(led_dev -> cls))
    {
        printk(KERN_ERR "class_create error\n");
        ret = PTR_ERR(led_dev->cls);//将指针出错的具体原因转换成一个出错码
        goto err_1;
    }
    led_dev->dev = device_create(led_dev->cls, NULL, MKDEV(led_dev->dev_major, 0), NULL, "led%d", 0);
    if(IS_ERR(led_dev -> dev))
    {
        printk(KERN_ERR "device_create error\n");
        ret = PTR_ERR(led_dev->dev);//将指针出错的具体原因转换成一个出错码
        goto err_2;
    }


/***********************对地址进行映射***********************************************/    
    led_dev->reg_virt_base = ioremap(GPX2_CON, GPX2_SIZE);
    if(led_dev->reg_virt_base==NULL)
    {
        printk(KERN_ERR "ioremap error\n");
        ret = -ENOMEM;
        goto err_3;
    }
/***********************配置gpio功能为输出***********************************************/
    u32 value = readl(led_dev->reg_virt_base);
    value &= ~(0xf << 28);
    value |= (0x1 << 28);
    writel(value, led_dev->reg_virt_base);
    
//递进执行下面标签中的语句，来逐级释放
err_3:
    device_destory(led_dev->cls, MKDEV(led_dev->dev_major,0));
err_2:
    class_destory(led_dev->cls);
err_1:
    unregister_chrdev(led_dev->dev_major, const char *name);
err_0:
    kfree(led_dev);
}


static void led_dev_exit(void)
{
    iounmap(led_dev->reg_virt_base);
    device_destory(led_dev->cls, MKDEV(led_dev->dev_major,0));
    class_destory(led_dev->cls);
    unregister_chrdev(led_dev->dev_major, const char *name);
    kfree(led_dev);
}
module_init(led_dev_init);
module_exit(led_dev_exit);
MODULE_LICENSE("GPL");