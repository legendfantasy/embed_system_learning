#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kfifo.h>

#include <linux/ioctl.h>
#include <linux/uaccess.h>

#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/aio.h>

#include "vser.h"

#define VSER_MAJOR	256
#define VSER_MINOR	0
#define VSER_DEV_CNT	1
#define VSER_DEV_NAME	"vser"

struct vser_dev {
	unsigned int baud;
	struct option opt;
	struct cdev cdev;
	wait_queue_head_t rwqh;
	wait_queue_head_t wwqh;
};

DEFINE_KFIFO(vsfifo, char, 32);
static struct vser_dev vsdev;

static int vser_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int vser_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t vser_read(struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
	int ret;
	unsigned int copied = 0;

	if (kfifo_is_empty(&vsfifo)) {
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;

		if (wait_event_interruptible_exclusive(vsdev.rwqh, !kfifo_is_empty(&vsfifo)))
			return -ERESTARTSYS;
	}

	ret = kfifo_to_user(&vsfifo, buf, count, &copied);

	if (!kfifo_is_full(&vsfifo))
		wake_up_interruptible(&vsdev.wwqh);

	return ret == 0 ? copied : ret;
}

static ssize_t vser_write(struct file *filp, const char __user *buf, size_t count, loff_t *pos)
{

	int ret;
	unsigned int copied = 0;

	if (kfifo_is_full(&vsfifo)) {
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;

		if (wait_event_interruptible_exclusive(vsdev.wwqh, !kfifo_is_full(&vsfifo)))
			return -ERESTARTSYS;
	}

	ret = kfifo_from_user(&vsfifo, buf, count, &copied);

	if (!kfifo_is_empty(&vsfifo))
		wake_up_interruptible(&vsdev.rwqh);

	return ret == 0 ? copied : ret;
}

static long vser_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	if (_IOC_TYPE(cmd) != VS_MAGIC)
		return -ENOTTY;

	switch (cmd) {
	case VS_SET_BAUD:
		vsdev.baud = arg;
		break;
	case VS_GET_BAUD:
		arg = vsdev.baud;
		break;
	case VS_SET_FFMT:
		if (copy_from_user(&vsdev.opt, (struct option __user *)arg, sizeof(struct option)))
			return -EFAULT;
		break;
	case VS_GET_FFMT:
		if (copy_to_user((struct option __user *)arg, &vsdev.opt, sizeof(struct option)))
			return -EFAULT;
		break;
	default:
		return -ENOTTY;
	}

	return 0;
}

static unsigned int vser_poll(struct file *filp, struct poll_table_struct *p)
{
	int mask = 0;

	poll_wait(filp, &vsdev.rwqh, p);
	poll_wait(filp, &vsdev.wwqh, p);

	if (!kfifo_is_empty(&vsfifo))
		mask |= POLLIN | POLLRDNORM;
	if (!kfifo_is_full(&vsfifo))
		mask |= POLLOUT | POLLWRNORM;

	return mask;
}

static ssize_t vser_aio_read(struct kiocb *iocb, const struct iovec *iov, unsigned long nr_segs, loff_t pos)
{
	size_t read = 0;
	unsigned long i;
	ssize_t ret;

	for (i = 0; i < nr_segs; i++) {
		ret = vser_read(iocb->ki_filp, iov[i].iov_base, iov[i].iov_len, &pos);
		if (ret < 0)
			break;
		read += ret;
	}

	return read ? read : -EFAULT;
}

static ssize_t vser_aio_write(struct kiocb *iocb, const struct iovec *iov, unsigned long nr_segs, loff_t pos)
{
// kiocb是异步IO控制块，iov是用户空间的缓冲区，nr_segs是缓冲区的数量，pos是文件的偏移量
	size_t written = 0;
	unsigned long i;
	ssize_t ret;

	for (i = 0; i < nr_segs; i++) {//遍历缓冲区
		ret = vser_write(iocb->ki_filp, iov[i].iov_base, iov[i].iov_len, &pos);
		//调用vser_write函数，将ki_filp指向的文件写入到缓冲区iov[i].iov_base中
		if (ret < 0)
			break;
		written += ret;
	}

	return written ? written : -EFAULT;
}

static struct file_operations vser_ops = {
	.owner = THIS_MODULE,
	.open = vser_open,
	.release = vser_release,
	.read = vser_read,
	.write = vser_write,
	.unlocked_ioctl = vser_ioctl,
	.poll = vser_poll,
	.aio_read = vser_aio_read,  // 异步读
	.aio_write = vser_aio_write,  //异步写
};

static int __init vser_init(void)
{
	int ret;
	dev_t dev;

	dev = MKDEV(VSER_MAJOR, VSER_MINOR);
	ret = register_chrdev_region(dev, VSER_DEV_CNT, VSER_DEV_NAME);
	if (ret)
		goto reg_err;

	cdev_init(&vsdev.cdev, &vser_ops);
	vsdev.cdev.owner = THIS_MODULE;
	vsdev.baud = 115200;
	vsdev.opt.datab = 8;
	vsdev.opt.parity = 0;
	vsdev.opt.stopb = 1;

	ret = cdev_add(&vsdev.cdev, dev, VSER_DEV_CNT);
	if (ret)
		goto add_err;

	init_waitqueue_head(&vsdev.rwqh);  // 初始化等待队列头
	init_waitqueue_head(&vsdev.wwqh);  // 初始化等待队列头

	return 0;

add_err:
	unregister_chrdev_region(dev, VSER_DEV_CNT);
reg_err:
	return ret;
}

static void __exit vser_exit(void)
{
	
	dev_t dev;

	dev = MKDEV(VSER_MAJOR, VSER_MINOR);

	cdev_del(&vsdev.cdev);
	unregister_chrdev_region(dev, VSER_DEV_CNT);
}

module_init(vser_init);
module_exit(vser_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kevin Jiang <jiangxg@farsight.com.cn>");
MODULE_DESCRIPTION("A simple character device driver");
MODULE_ALIAS("virtual-serial");
