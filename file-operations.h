#ifndef F_OPS_H
#define F_OPS_H

int     dev_open(struct inode *, struct file *);
int     dev_release(struct inode *, struct file *);
ssize_t dev_read(struct file *, char *, size_t, loff_t *);
ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

#endif
