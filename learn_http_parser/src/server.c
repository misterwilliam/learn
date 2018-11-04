#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <http_parser.h>

#define PORT 8080
#define MAXBUF 1024

int on_message_begin(http_parser* _) {
  (void)_;
  printf("\n***MESSAGE BEGIN***\n\n");
  return 0;
}

int on_headers_complete(http_parser* _) {
  (void)_;
  printf("\n***HEADERS COMPLETE***\n\n");
  return 0;
}

int on_message_complete(http_parser* _) {
  (void)_;
  printf("\n***MESSAGE COMPLETE***\n\n");
  return 0;
}

int on_url(http_parser* _, const char* at, size_t length) {
  (void)_;
  printf("Url: %.*s\n", (int)length, at);
  return 0;
}

int on_header_field(http_parser* _, const char* at, size_t length) {
  (void)_;
  printf("Header field: %.*s\n", (int)length, at);
  return 0;
}

int on_header_value(http_parser* _, const char* at, size_t length) {
  (void)_;
  printf("Header value: %.*s\n", (int)length, at);
  return 0;
}

int on_body(http_parser* _, const char* at, size_t length) {
  (void)_;
  printf("Body: %.*s\n", (int)length, at);
  return 0;
}

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

int main(int argc, char **argv) {
  int listen_fd = -1;
  listen_fd = socket(AF_INET6, SOCK_STREAM, 0);
  if (listen_fd < 0) {
    perror("Socket create");
    exit(errno);
  }

  // Create IPv6 sockaddr.
  struct sockaddr_in6 server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin6_family = AF_INET6;
  server_addr.sin6_port = htons(PORT);
  server_addr.sin6_addr = in6addr_any;

  // Bind to sockaddr.
  if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
      perror("Socket bind");
      exit(errno);
  }

  // Listen.
  if (listen(listen_fd, 20 /* backlog */) != 0) {
    perror("Socket listen");
    exit(errno);
  }

  // Init HTTP parser.
  http_parser_settings settings;
  memset(&settings, 0, sizeof(settings));
  settings.on_message_begin = on_message_begin;
  settings.on_url = on_url;
  settings.on_header_field = on_header_field;
  settings.on_header_value = on_header_value;
  settings.on_headers_complete = on_headers_complete;
  settings.on_body = on_body;
  settings.on_message_complete = on_message_complete;
  http_parser parser;
  http_parser_init(&parser, HTTP_REQUEST);

  char buf[MAXBUF];

  while (true) {
    // Accept connections.
    struct sockaddr_in6 client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);

    // Get presentation form of client IPv6 IP.
    char paddr[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &client_addr.sin6_addr, paddr, sizeof(paddr));
    printf("[%s]:%d connected\n", paddr, ntohs(client_addr.sin6_port));

    // Read from socket.
    int nrecv = recv(client_fd, buf, MAXBUF, 0);
    if (nrecv == -1) {
      perror("Socket recv");
      exit(errno);
    }
    printf("Recv: %.*s\n", nrecv, buf);

    // Parse request.
    // Note that case nrecv == 0 is passed along to parser to indicate when EOF
    // has been read.
    int nparsed = http_parser_execute(&parser, &settings, buf, nrecv);
    if (parser.upgrade) {
      printf("HTTP Upgrade\n");
    } else if (nparsed != nrecv) {
      printf("HTTP Parse error: %s (%s).\n",
             http_errno_description(HTTP_PARSER_ERRNO(&parser)),
             http_errno_name(HTTP_PARSER_ERRNO(&parser)));

      if (shutdown(client_fd, SHUT_RDWR) != 0) {
        perror("Socket shutdown");
        exit(errno);
      }
      break;
    }

    char* resp = "hello world\n";
    write_http_resp(client_fd, resp, strlen(resp));

    // Close TCP connection.
    if (shutdown(client_fd, SHUT_RDWR) != 0) {
      perror("Socket shutdown");
      exit(errno);
    }
    printf("[%s]:%d closed\n", paddr, ntohs(client_addr.sin6_port));
  }

  return EXIT_SUCCESS;
}
