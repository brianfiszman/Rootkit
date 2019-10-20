#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include "device-init.h"

static int major;

void print_major_number(dev_t *dev_num, char *dev_name) {
    major = MAJOR(*dev_num);
    printk(KERN_INFO "%s: major number is %d", dev_name, major);
    printk(KERN_INFO "\tuse \"use mknod /dev/%s c %d 0\" for device file", dev_name, major);
}
