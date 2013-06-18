#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
 
#define DEVICE_NAME "sfusion"
#define BUFFER_SIZE 1024
#define WORD_SEPERATOR ' '
#define VALUE_SEPERATOR ','
 
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Rony Fragin & Shai Fogel");
MODULE_DESCRIPTION("Saiyan Fusion Helper.");
MODULE_SUPPORTED_DEVICE(DEVICE_NAME);
 
int device_init(void);
void device_exit(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t get_next_word(const char *input, size_t length, char *output, const int seperator);
static bool strmatch(const char* str1, const char* str2);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
 
module_init(device_init);
module_exit(device_exit);
 
static struct file_operations fops = {
.read = device_read,
.write = device_write,
.open = device_open,
.release = device_release
};
 
static int device_major = -1;
static int device_opend = 0;
static char device_buffer[BUFFER_SIZE];
static char *buff_rptr;
static char *buff_wptr;
 
int device_init(void) {
	device_major = register_chrdev(0, DEVICE_NAME, &fops);
	if(device_major < 0) {
		printk(KERN_ALERT "sfusion: cannot obtain major number %d.\n", device_major);
		return device_major;
	}
	memset(device_buffer, 0, BUFFER_SIZE);
	printk(KERN_INFO "sfusion: sfusion loaded with major %d.\n", device_major);
	return 0;
}
 
void device_exit(void) {
	unregister_chrdev(device_major, DEVICE_NAME);
	printk(KERN_INFO "sufsion: sfusion unloaded.\n");
}
 
static int device_open(struct inode *nd, struct file *fp) {
	if(device_opend) return -EBUSY;
	device_opend++;
	buff_rptr = buff_wptr = device_buffer;
	try_module_get(THIS_MODULE);
	return 0;
}
 
static int device_release(struct inode *nd, struct file *fp) {
	if(device_opend) device_opend--;
	module_put(THIS_MODULE);
	return 0;
}
 
static ssize_t device_read(struct file *fp, char *buff, size_t length, loff_t *offset) {
	char *msg = "reading from the sfusion device";
	copy_to_user(buff, msg, strlen(msg));
	return strlen(msg);
}
 
static ssize_t get_next_word(const char *input, size_t length, char *output, const int seperator)
{
	// Find first occurrence of the word seperator.
	char *found = strnchr(input, length, seperator);
	int offset = 0;
	if(found == NULL)
		return offset;
	// Get word's length.
	offset = found - input;
	//printk(KERN_INFO "sfusion: input %s.\n", input);
	//printk(KERN_INFO "sfusion: offset %d length %d seperator %c.\n", offset, length, WORD_SEPERATOR);
	//printk(KERN_INFO "sfusion: found %s.\n", found);
	// Copy the word.
	memcpy(output, input, offset);
	// Add null terminator to the string.
	output[offset] = '\0';
	return offset + 1;
}

static bool strmatch(const char* str1, const char* str2)
{
	return (strcmp(str1, str2) == 0);
}

static ssize_t device_write(struct file *fp, const char *buff, size_t length, loff_t *offset) {
	char *ptr;
	int word_length = 0;
	//printk(KERN_INFO "sfusion: text %s.\n", buff);
	// Allocate space for the buffer.
	ptr = kmalloc(length * sizeof(char), GFP_KERNEL);
	// Get first word.
	word_length = get_next_word(buff, length, ptr, WORD_SEPERATOR);
	// Word wasn't found.
	if(word_length == 0)
			goto reading_failed;
	// Check if word equals set.
	if(strmatch(ptr, "set"))
	{
		//printk(KERN_INFO "sfusion: sub_text %s.\n", ptr);
		// Move to the next word.
		word_length = get_next_word(buff + word_length, length, ptr, WORD_SEPERATOR);
		// Word wasn't found.
		if(word_length == 0)
			goto reading_failed;
		// Check if word equals devices.
		if(strmatch(ptr, "devices"))
		{
			//printk(KERN_INFO "sfusion: sub_text %s.\n", ptr);
			word_length = get_next_word(buff + word_length, length, ptr, WORD_SEPERATOR);
			// Word wasn't found.
			if(word_length == 0)
				goto reading_failed;
			while(get_next_word(buff + word_length, length, ptr, VALUE_SEPERATOR) != 0)
			{
				printk(KERN_INFO "sfusion: sub_text %s.\n", ptr);
			}
		}
		else
			goto reading_failed;
	}
	else if(strmatch(ptr, "add"))
	{

	}
	else if(strmatch(ptr, "remove"))
	{

	}
	else
		goto reading_failed;
	goto success;
	reading_failed:
	printk(KERN_INFO "sfusion: Illegal string was entered.\n");
	success:
	kfree(ptr);
	return length;
}
