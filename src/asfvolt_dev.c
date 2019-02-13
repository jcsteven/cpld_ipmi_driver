#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
//#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include "asfvolt_dev.h"

extern int asxvolt16_cpld_read(unsigned short cpld_addr, u8 reg);
extern int asxvolt16_cpld_write(unsigned short cpld_addr, u8 reg, u8 value);

/* uncomment the next line to enable debug output to the kernel CLI */
#define ASFVOLT_DEV_DEBUG_PRINTS 1
#ifdef ASFVOLT_DEV_DEBUG_PRINTS
#define asfvolt_dev_debug_print(fmt, args...) printk("%s#%d: " fmt, __FUNCTION__, __LINE__, ## args)
#else
#define asfvolt_dev_debug_print(...)
#endif


#define DEV_NAME "dev_asfvolt"
#define FIRST_MINOR 0
#define MINOR_CNT 1



static int dev_major =   0;
static int dev_minor =   0;


static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

cpld_data_arg_t dev_data={ 0x62,0x02,0x00};


void print_cpld_Data( char *pMsg,cpld_data_arg_t *pData)
{
    asfvolt_dev_debug_print("%s::CPLD Data: address=%02X, reg=%02X  value=%02X\n",pMsg,pData->address,pData->reg,pData->value);	
}


static int dev_open(struct inode *i, struct file *f)
{
    asfvolt_dev_debug_print("Device has been opened.\n");
    return 0;
}
static int dev_close(struct inode *i, struct file *f)
{
    asfvolt_dev_debug_print("Device successfully closed.\n");
    return 0;
}

static long dev_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
   int status;

    switch (cmd)
    {
        case CPLD_DATA_GET_VARIABLES:
            if (copy_from_user(&dev_data, (cpld_data_arg_t *)arg, sizeof(cpld_data_arg_t)))
            {
                printk(KERN_ERR "%s:%i-Error: Fail to copy_from_user\n", __FUNCTION__,__LINE__);
                return -EACCES;
            }
            print_cpld_Data("Reciving",&dev_data);
            status = asxvolt16_cpld_read(dev_data.address,dev_data.reg);
            if (status < 0)
            {
                printk(KERN_ERR "%s:%i-Error: Fail to asxvolt16_cpld_read\n", __FUNCTION__,__LINE__);
                return status;
            }
            dev_data.value=( unsigned char)status;
            print_cpld_Data("Sending",&dev_data);
            if (copy_to_user((cpld_data_arg_t *)arg, &dev_data, sizeof(cpld_data_arg_t)))
            {
                printk(KERN_ERR "%s:%i-Error: Fail to copy_to_user\n", __FUNCTION__,__LINE__);
                return -EACCES;
            }
            break;
        case CPLD_DATA_SET_VARIABLES:
            if (copy_from_user(&dev_data, (cpld_data_arg_t *)arg, sizeof(cpld_data_arg_t)))
            {
                printk(KERN_ERR "%s:%i-Error: Fail to copy_from_user\n", __FUNCTION__,__LINE__);
                return -EACCES;
            }
            print_cpld_Data("Reciving",&dev_data);
            status=asxvolt16_cpld_write(dev_data.address,dev_data.reg, dev_data.value);
            if (status < 0)
            {
                printk(KERN_ERR "%s:%i-Error: Fail to asxvolt16_cpld_write\n", __FUNCTION__,__LINE__);
                return status;
            }

            break;
        default:
            return -EINVAL;
    }

    return 0;
}


static struct file_operations dev_fops =
{
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_close,
    .unlocked_ioctl = dev_ioctl
};

static int __init dev_cpld_init(void)
{
    int ret;
    struct device *dev_ret;
    asfvolt_dev_debug_print("Device init:Start.\n");
    if (dev_major) {
        dev = MKDEV(dev_major, dev_minor);
        ret = register_chrdev_region(dev, MINOR_CNT, DEV_NAME);
    } else {
        ret = alloc_chrdev_region(&dev, dev_minor, MINOR_CNT,DEV_NAME);
        dev_major = MAJOR(dev);
    }
    if (ret < 0) {
        printk(KERN_ERR "%s:%i-Error: Fail to alloc_chrdev_region. dev_nsme:%s: can't get major %d \n", __FUNCTION__,__LINE__,DEV_NAME,dev_major);
        return ret;
    }

    cdev_init(&c_dev, &dev_fops);

    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
    {
        printk(KERN_ERR "%s:%i-Error: Fail to cdev_add.\n", __FUNCTION__,__LINE__);
        return ret;
    }

    if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
    {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        printk(KERN_ERR "%s:%i-Error: Fail to class_create.\n", __FUNCTION__,__LINE__);
        return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "dev_asfvolt")))
    {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        printk(KERN_ERR "%s:%i-Error: Fail to device_create.\n", __FUNCTION__,__LINE__);
        return PTR_ERR(dev_ret);
    }

    asfvolt_dev_debug_print("Device init:End\n");
    return 0;
}

static void __exit dev_cpld_exit(void)
{
    asfvolt_dev_debug_print("Device exit:Start\n");
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
    asfvolt_dev_debug_print("Device exit:End\n");
}

module_init(dev_cpld_init);
module_exit(dev_cpld_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("JC Yu <email:jcsteven.yu@gmail.com>");
MODULE_DESCRIPTION("This is driver to communicate with asxvolt-cpld driver");
MODULE_VERSION("1.0");
