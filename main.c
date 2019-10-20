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
#include "file-operations.h"
#include "device-init.h"

MODULE_LICENSE("GPL");

#define DEVICE_NAME "TestDevice"  // name--> appears in /proc/devices
#define CLASS_NAME "Test"         ///< The device class

static int uevent(struct device *dev, struct kobj_uevent_env *env) {
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
int ret;  // will be used to hold return values of functions; this is because
          // the kernel stack is very small so declaring variables all over the
          // pass in our module functions eats up the stack very fast
dev_t dev_num;  // will hold major number that kernel gives us

static int __init test_init(void) {
  ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);

  // Create a class which will appear at /sys/class
  devClass             = class_create(THIS_MODULE, CLASS_NAME);
  devClass->dev_uevent = uevent;

  cdevice = device_create(devClass, NULL, dev_num, NULL, DEVICE_NAME);

  if (ret < 0) {
    printk(KERN_ALERT "Load failed\n");
    return -1;
  }

  print_major_number(&dev_num, DEVICE_NAME);

  mcdev        = cdev_alloc();

  cdev_init(mcdev, &fops);
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

module_init(test_init);
module_exit(test_exit);
