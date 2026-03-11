#include <errno.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x));

int tun_alloc(char *dev) {
  struct ifreq ifr;
  int fd, err;

  if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
    perror("Cannot open TUN/TAP dev");
    exit(EXIT_FAILURE);
  }

  CLEAR(ifr);

  ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
  if (*dev) {
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  }

  if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
    perror("ERR: Could not ioctl tun");
    close(fd);
    return err;
  }

  strcpy(dev, ifr.ifr_name);
  return fd;
}

int main(void) {
  char dev[IFNAMSIZ] = "test"; // writable buffer on the stack
  int fd = tun_alloc(dev);
  if (fd < 0) {
    fprintf(stderr, "Failed to allocate TAP device\n");
    return 1;
  }

  if (ioctl(fd, TUNSETPERSIST, 1) < 0) {
    perror("ERR: Could not set persistent");
    close(fd);
    return 1;
  }

  printf("TAP device created: %s (fd=%d)\n", dev, fd);
  close(fd);
  return 0;
}
