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

void handle_command(char* input);
const char* CHANGE_FILENAME =  "/change_filename ";
static dev_t first;
static struct cdev c_dev; 
static struct class *cl;
static struct proc_dir_entry* entry;


struct res_vector
{
	int *data;
	size_t size;
	size_t idx;
};

struct res_vector rv;

struct res_vector rv_init(void)
{
	return (struct res_vector) {
		.data = vmalloc(sizeof(int)),
		.size = 1,
		.idx = 0,
	};
}

void rv_append(size_t value, struct res_vector *r_vector)
{
	if (r_vector->idx == r_vector->size - 1)
	{
		r_vector->size *= 2;
		int* n_data = (int*)vmalloc(r_vector->size * sizeof(int));
		memcpy(n_data, r_vector->data, r_vector->idx * sizeof(int));
		vfree(r_vector->data);
		r_vector->data = n_data;
	}
	r_vector->data[r_vector->idx++] = value;
}

int count_letters(const char* string) {
	char* cur = string;
	int res = 0;
	while (*cur != '\0' ) {
		if ((*cur >= 'A' && *cur <= 'Z') || 
		     (*cur >= 'a' && *cur <= 'z')) {
			res++;
		}
		cur++;
	}
	return res;
}


static ssize_t proc_write(struct file *file, const char __user * ubuf, size_t count, loff_t* ppos) 
{
	printk(KERN_DEBUG "Attempt to write proc file");
  	return -1;
}


static ssize_t proc_read(struct file *file, char __user * ubuf, size_t count, loff_t* ppos) 
{
  	char sarr[512];
  	int written = 0;
  	int i = 0;
  	size_t len;

  	for (i = 0; i < rv.idx; i++)
    		written += snprintf(&sarr[written], 512 - written, "%d - %d\n", i+1, rv.data[i]);
  	sarr[written] = 0;

       	len = strlen(sarr);
  	if (*ppos > 0 || count < len) return 0;
  	if (copy_to_user(ubuf, sarr, len) != 0) return -EFAULT;
  	*ppos = len;
  	return len;
}

static int my_open(struct inode *i, struct file *f)
{
  	return 0;
}

static int my_close(struct inode *i, struct file *f)
{
  	return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	int i = 0;
  	for(i = 0; i < rv.idx; i++)
		printk(KERN_DEBUG "%d - %d\n", i+1, rv.data[i]);
	return 0;
}

static ssize_t my_write(struct file *f, const char __user *buf,  size_t len, loff_t *off)
{
	char* user_input = (char*)vmalloc(len * sizeof(char));
	memcpy(user_input, buf, len * sizeof(char));
	user_input[len] = 0;
	if (*user_input == '/') {
		handle_command(user_input);
	} else {
		int res = count_letters(user_input);
		rv_append(res, &rv);
	}
	*off += len;
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

void handle_command(char* input){
	int i = 0;
	for (; i < 17; ++i) {
		if (*input != CHANGE_FILENAME[i]) return;
		input++;
	}

	while (*input == ' ') {
		input++;
	}

	proc_remove(entry);

	char* cur = input;
	while(*cur != '\n') {
		cur++;
	} 
	*cur = 0;
	entry = proc_create(input, 0444, NULL, &fops);
	printk(KERN_INFO "created new file '%s'\n", input);		
}

 
static int __init ch_drv_init(void)
{
    	printk(KERN_INFO "Hello!\n");
	
	rv = rv_init();

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

