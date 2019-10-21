#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include "device-init.h"

static int major;
volatile static int ret;

static int uevent(struct device *dev, struct kobj_uevent_env *env) {
  add_uevent_var(env, "DEVMODE=%#o", 0666);
  return 0;
}

struct class* init_char_device(dev_t *        dev_num,
			       struct class * dev_class,
			       struct device *cdevice,
			       char *         dev_name,
			       char *         class_name) {
  ret = alloc_chrdev_region(dev_num, 0, 1, dev_name);

  // Create a class which will appear at /sys/class
  dev_class             = class_create(THIS_MODULE, class_name);
  dev_class->dev_uevent = uevent;

  cdevice = device_create(dev_class, NULL, *dev_num, NULL, dev_name);

  if (ret < 0) {
    printk(KERN_ALERT "Load failed\n");
    return dev_class;
  }

  return dev_class;
}

void print_major_number(dev_t *dev_num, char *dev_name) {
  major = MAJOR(*dev_num);
  printk(KERN_INFO "%s: major number is %d", dev_name, major);
  printk(KERN_INFO "\tuse \"use mknod /dev/%s c %d 0\" for device file",
         dev_name,
         major);
}
