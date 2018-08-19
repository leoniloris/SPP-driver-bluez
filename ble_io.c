#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

#define BUFFER_SIZE 1024
#define MAJOR_NUMBER 7

MODULE_LICENSE("GPL");

static int buffer_offset = 0;

char *DEVICE_NAME = "char_driver";

char *device_buffer;
static int closeCounter = 0;
static int openCounter = 0;

ssize_t char_driver_read(struct file *pfile, char __user *buffer,
                                size_t length, loff_t *offset) {
  if (*offset > BUFFER_SIZE) {
    printk("offset is greater than buffer size");
    return 0;
  }
  if (*offset + length > BUFFER_SIZE) {
    length = BUFFER_SIZE - *offset;
  }
  if (copy_to_user(buffer, device_buffer + *offset, length) != 0) {
    return -EFAULT;
  }
  *offset += length;
  return length;
}

ssize_t char_driver_write(struct file *pfile, const char __user *buffer,
                                 size_t length, loff_t *offset) {
  int nb_bytes_to_copy;
  if (BUFFER_SIZE - 1 - *offset <= length) {
    nb_bytes_to_copy = BUFFER_SIZE - 1 - *offset;
    printk("BUFFER_SIZE - 1 -*offset <= length");
  } else if (BUFFER_SIZE - 1 - *offset > length) {
    nb_bytes_to_copy = length;
    printk("BUFFER_SIZE - 1 -*offset > length");
  }
  printk(KERN_INFO "Writing to device\n");
  if (*offset + length > BUFFER_SIZE) {
    printk("sorry, can't do that. ");
    return -1;
  }
  printk("about to copy from device");
  copy_from_user(device_buffer + *offset, buffer, nb_bytes_to_copy);
  device_buffer[*offset + nb_bytes_to_copy] = '\0';
  *offset += nb_bytes_to_copy;
  return nb_bytes_to_copy;
}

int char_driver_open(struct inode *pinode, struct file *pfile) {
  printk(KERN_ALERT "This device is now open");
  openCounter++;
  printk(KERN_ALERT "This device has been opened this many times: %d\n",
         openCounter);
  return 0;
}

int char_driver_close(struct inode *pinode, struct file *pfile) {
  printk(KERN_ALERT "This device is now closed");
  closeCounter++;
  printk(KERN_ALERT "This device has been closed this many times: %d\n",
         closeCounter);
  return 0;
}

loff_t char_driver_seek(struct file *pfile, loff_t offset, int whence) {
  printk(KERN_ALERT "We are now seeking!");
  switch (whence) {
  case 0: {
    if (offset <= BUFFER_SIZE && offset > 0) {
      buffer_offset = offset;
      printk(KERN_ALERT " this is where we are in the buffer: %d\n",
             buffer_offset);
    } else {
      printk(KERN_ALERT "ERROR you are attempting to go ouside the Buffer");
    }
    break; // THIS IS SEEK_SET
  }

  case 1: {
    if (((buffer_offset + offset) <= BUFFER_SIZE) &&
        ((buffer_offset + offset) > 0)) {
      buffer_offset = buffer_offset + offset;
      printk(KERN_ALERT " this is where we are in the buffer: %d\n",
             buffer_offset);
    } else {
      printk(KERN_ALERT "ERROR you are attempting to go ouside the Buffer");
    }
    break;
  }
  case 2: { // THIS IS SEEK END
    if ((BUFFER_SIZE - offset) >= 0 && offset > 0) {
      buffer_offset = BUFFER_SIZE - offset;
      printk(KERN_ALERT " this is where we are in the buffer: %d\n",
             buffer_offset);
    } else {
      printk(KERN_ALERT "ERROR you are attempting to go ouside the Buffer");
    }
    break;
  }
  default: {}
  }
  printk(KERN_ALERT "I sought %d\n", whence);
  return buffer_offset;
}

struct file_operations char_driver_file_operations = {
    .owner = THIS_MODULE,
    .read = char_driver_read,
    .write = char_driver_write,
    .open = char_driver_open,
    .llseek = &char_driver_seek,
    .release = char_driver_close,
};

static int char_driver_init(void) {
  printk(KERN_ALERT "inside %s function\n", __FUNCTION__);
  register_chrdev(MAJOR_NUMBER, DEVICE_NAME,
                  &char_driver_file_operations);
  device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
  return 0;
}

static void char_driver_exit(void) {
  printk(KERN_ALERT "inside %s function\n", __FUNCTION__);
  unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME);
  kfree(device_buffer);
}

module_init(char_driver_init);
module_exit(char_driver_exit);
