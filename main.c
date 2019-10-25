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
#include "device-init.h"

MODULE_LICENSE("GPL");

#define DEVICE_NAME "TestDevice"  // name--> appears in /proc/devices
#define CLASS_NAME "Test"         ///< The device class

struct char_device* _chdev;
volatile static int ret;  // will be used to hold return values of functions; this is because
          // the kernel stack is very small so declaring variables all over the
          // pass in our module functions eats up the stack very fast

static int __init test_init(void) {
  _chdev = kmalloc(sizeof(struct char_device), GFP_KERNEL);
  
  ret = init_char_device(_chdev, DEVICE_NAME, CLASS_NAME);

  print_major_number(&_chdev->dev_num, DEVICE_NAME);

  alloc_char_device(_chdev);

  if (ret < 0) {
    printk(KERN_ALERT "%s: unable to add cdev to kernel", DEVICE_NAME);
    return ret;
  }

  printk(KERN_INFO "Module has been loaded\n");
  return 0;
}

static void __exit test_exit(void) {
  clean_device(_chdev);
  printk(KERN_INFO "Module has been unloaded\n");
}

module_init(test_init);
module_exit(test_exit);
