#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <linux/if_tun.h>
#include <net/if.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

// BUFSIZE needs to be larger than MTU.
#define BUFSIZE 2000

int tun_alloc(char *dev, int flags) {
  int fd, err;
  if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
    perror("Opening /dev/net/tun");
    return fd;
  }

  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = flags;

  if (*dev) {
    strncpy(ifr.ifr_name, dev, IFNAMSIZ-1);
  }

  if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
    perror("ioctl(TUNSETIFF)");
    close(fd);
    return err;
  }

  strcpy(dev, ifr.ifr_name);

  return fd;
}

int main(int argc, char **argv) {
  // IFNAMSIZ is the maximum buffer size needed to hold an interface name.
  char tun_name[IFNAMSIZ];

  strcpy(tun_name, "tun0");
  int tun_fd = tun_alloc(tun_name, IFF_TUN | IFF_NO_PI);
  if (tun_fd < 0) {
    perror("Unable to allocate tun");
    exit(1);
  }
  printf("Attached to tun: %s\n", tun_name);

  char buf[BUFSIZE];
  while(1) {
    int nread = read(tun_fd, buf, sizeof(buf));
    if (nread < 0) {
      perror("Reading from tun");
      close(tun_fd);
      exit(1);
    }

    printf("Read: %d bytes\n", nread);
  }

  printf("tun_fd=%d\n", tun_fd);
  close(tun_fd);

  return 0;
}
