#include "tcp_server.h"

#define PORT 8080
#define MAXBUF 1024

bool send_resp(int fd, const char *mesg, size_t len) {
  ssize_t sent_bytes = 0;
  while (true) {
    int wrote = send(fd, mesg + sent_bytes, len - sent_bytes, 0);
    if (wrote == -1) {
      if (errno == EINTR) {
        continue;
      }
      return false;
    }

    printf("Send: %.*s\n", wrote, mesg + sent_bytes);

    sent_bytes += wrote;
    if (sent_bytes == len) {
      return true;
    }
  }
}

bool write_http_resp(int fd, const char *mesg, size_t len) {
  const char* preamble = "HTTP/1.1 200 OK\r\nContent-Length: ";
  bool ok = send_resp(fd, preamble, strlen(preamble));
  if (!ok) {
    perror("Socket send");
    return false;
  }

  char content_len_buf[256];
  snprintf(content_len_buf, sizeof content_len_buf, "%zu", len);
  ok = send_resp(fd, content_len_buf, strlen(content_len_buf));
  if (!ok) {
    perror("Socket send");
    return false;
  }

  char* double_crlf = "\r\n\r\n";
  ok = send_resp(fd, double_crlf, strlen(double_crlf));
  if (!ok) {
    perror("Socket send");
    return false;
  }

  ok = send_resp(fd, mesg, len);
  if (!ok) {
    perror("Socket send");
    return false;
  }

  return true;
}

bool on_recv(const char* buf, size_t len, int client_fd)  {
  printf("Recv: %.*s\n", (int)len, buf);

  char* resp = "hello world\n";
  write_http_resp(client_fd, resp, strlen(resp));
  return true;
};

int main(int argc, char **argv) {
  struct Server srvr;
  srvr.on_recv = on_recv;
  Init(&srvr, MAXBUF);

  bool ok = Listen(&srvr, PORT);
  if (!ok) {
    exit(EXIT_FAILURE);
  }
  ok = Serve(&srvr);
  if (!ok) {
    Destroy(&srvr);
    exit(EXIT_FAILURE);
  }

  Destroy(&srvr);
  return EXIT_SUCCESS;
}
