#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#define BUF_LEN 100
#define MYCHAR_DEV_CNT 3  //设备个数

int mychar_num  = MYCHAR_DEV_CNT;
int major = 0;
int minor = 0;

struct mychar_dev  
{
    struct cdev mydev; //cdev对象
    char mydev_buf[BUF_LEN];  //设备的缓冲区
    int curlen;  //缓冲区中有效数据的长度
};
struct mychar_dev gmydev; 

int mychar_open(struct inode *pnode, struct file *pfile)
{
    pfile->private_data = (void *) (container_of(pnode->i_cdev, struct mychar_dev, mydev));
    printk("mychar_open is called\n");
    return 0;
}

int mychar_close(struct inode *pnode,struct file *pfile)
{
    printk("mychar_close is called\n");
    return 0;
}

ssize_t mychar_read(struct file *pfile,char __user *puser,size_t count,loff_t *p_pos)
{
    struct mychar_dev *pmydev = (struct mychar_dev *)pfile->private_data;
    int size = 0;
    int ret = 0;

    if(count > pmydev->curlen)
    {
        size = pmydev->curlen;
    }
    else
    {
        size = count;
    }

    ret = copy_to_user(puser, pmydev->mydev_buf, size);
    if(ret == 0)
    {
        printk("read %d bytes from kernel space\n", size);
        return size;
    }
    else
    {
        printk("copy_to_user failed\n");
        return -EFAULT;
    }
}
ssize_t mychar_write(struct file *pfile, const char __user *puser, size_t count, loff_t *p_pos)
{
    struct mychar_dev *pmydev = (struct mychar_dev *)pfile->private_data;
    int size = 0;
    int ret = 0;

    if(count > BUF_LEN)
    {
        size = BUF_LEN;
    }
    else
    {
        size = count;
    }

    ret = copy_from_user(pmydev->mydev_buf, puser, size);
    if(ret == 0)
    {
        printk("write %d bytes to kernel space\n", size);
        pmydev->curlen = size;
        return size;
    }
    else
    {
        printk("copy_from_user failed\n");
        return -EFAULT;
    }
}

struct file_operations myfops = {
    .owner = THIS_MODULE,
    .open = mychar_open,
    .release = mychar_close,
    .read = mychar_read,
    .write = mychar_write,
};

int __init mychar_init(void)
{
    if(major)  //如果指定了主设备号
    {
        dev_t devno = MKDEV(major, minor);
        int ret = register_chrdev_region(devno, mychar_num, "mychar");
        if(ret < 0)
        {
            printk("register_chrdev_region failed\n");
            return ret;
        }
    }
    else
    {
        dev_t devno;
        int ret = alloc_chrdev_region(&devno, 0, mychar_num, "mychar");
        if(ret < 0)
        {
            printk("alloc_chrdev_region failed\n");
            return ret;
        }
        major = MAJOR(devno);
    }

    cdev_init(&gmydev.mydev, &myfops);
    int ret = cdev_add(&gmydev.mydev, dev, mychar_num);
    if(ret < 0)
    {
        printk("cdev_add failed\n");
        return ret;
    }


    return 0;
}

void __exit mychar_exit(void)
{
    dev_t devno = MKDEV(major, minor);
    for(int i = 0; i < mychar_num; i++)
    {
        cdev_del(&gmydev[i].mydev);
    }
    unregister_chrdev_region(devno, mychar_num);
}



MODULE_LICENSE("GPL");
module_init(mychar_init);
module_exit(mychar_exit);