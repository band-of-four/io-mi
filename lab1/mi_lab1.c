#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>

#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/init.h>

static dev_t first;
static struct cdev c_dev; 
static struct class *cl;
static struct proc_dir_entry* entry;

static int *result_array;
static int result_len = 0;

static ssize_t proc_write(struct file *file, const char __user * ubuf, size_t count, loff_t* ppos) 
{
	printk(KERN_DEBUG "Attempt to write proc file");
  	return -1;
}


static ssize_t proc_read(struct file *file, char __user * ubuf, size_t count, loff_t* ppos) 
{
  	char sarr[count];
  	int written = 0;
  	int i = 0;
  	for (i = 0; i < result_len; i++)
    		written += snprintf(&sarr[written], count, "%d\n", result_array[i]);
  	sarr[written] = 0;

  	size_t len = strlen(sarr);
  	if (*ppos > 0 || count < len) return 0;
  	if (copy_to_user(ubuf, sarr, len) != 0) return -EFAULT;
  	if (len != 0) *ppos = len;
  	return len;
}

static int my_open(struct inode *i, struct file *f)
{
  	printk(KERN_INFO "Driver: open()\n");
  	return 0;
}

static int my_close(struct inode *i, struct file *f)
{
  	printk(KERN_INFO "Driver: close()\n");
  	return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
  	printk(KERN_INFO "Driver: read()\n");
  	return 0;
}

static ssize_t my_write(struct file *f, const char __user *buf,  size_t len, loff_t *off)
{
  	char *s = buf;
  	s[len] = 0;

  	int res = len - 1;    

  	char *format = KERN_INFO "%s";
  	char *format2 = KERN_INFO "len=%d";
  	printk(format, s);
  	printk(format2, (int)res);
  

  	if (result_len < 64) {
    		result_array[result_len++] = res;
  	} else {
    		int i = 0;
    		for (; i < 63; ++i) result_array[i] = result_array[i+1];
    		result_array[result_len] = res;
  	}

  	printk(KERN_INFO "Driver: write()\n");
  	return len;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = proc_read,
	.write = proc_write,
};

static struct file_operations mychdev_fops =
{
  	.owner = THIS_MODULE,
  	.open = my_open,
  	.release = my_close,
  	.read = my_read,
  	.write = my_write
};
 
static int __init ch_drv_init(void)
{
    	printk(KERN_INFO "Hello!\n");

    	result_array = (int*)kmalloc(64 * sizeof(int), GFP_KERNEL);

    	entry = proc_create("var5", 0444, NULL, &fops);
    	if (alloc_chrdev_region(&first, 0, 1, "ch_dev") < 0) return -1;
    	if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
	{
		unregister_chrdev_region(first, 1);
		return -1;
	}
    	if (device_create(cl, NULL, first, NULL, "mychdev") == NULL)
    	{
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
    	}
    	cdev_init(&c_dev, &mychdev_fops);
    	if (cdev_add(&c_dev, first, 1) == -1)
	{
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	}
    	return 0;
}
 
static void __exit ch_drv_exit(void)
{
    	proc_remove(entry);
    	cdev_del(&c_dev);
    	device_destroy(cl, first);
    	class_destroy(cl);
    	unregister_chrdev_region(first, 1);
    	printk(KERN_INFO "Bye!!!\n");
}
 
module_init(ch_drv_init);
module_exit(ch_drv_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kalugina & Keker");
MODULE_DESCRIPTION("The lab 1 kernel module");

