#ifndef D_INIT_H
#define D_INIT_H

#include <linux/cdev.h>

struct char_device {
  struct device *cdevice;
  struct class *dev_class;
  struct cdev *mcdev;
  dev_t dev_num;  // will hold major number that kernel gives us
};

int init_char_device(struct char_device *_chdev,
		     char *         dev_name,
		     char *         class_name);
void print_major_number(dev_t *, char *);

#endif
