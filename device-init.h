#ifndef D_INIT_H
#define D_INIT_H

#include <linux/cdev.h>

struct class*  init_char_device(dev_t *        dev_num,
				struct class  *dev_class,
				struct device *cdevice,
				char *         dev_name,
				char *         class_name);
void print_major_number(dev_t *, char *);

#endif
