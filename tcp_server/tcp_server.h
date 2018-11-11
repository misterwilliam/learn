#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

struct Server {
  char* buf;
  size_t max_buf;
  int listen_fd;

  // Public:
  bool (*on_recv)(const char* buf, size_t len, int client_fd);
};

void Init(struct Server *srvr, size_t max_buf) {
  srvr->buf = malloc(sizeof(char) * max_buf);
  srvr->max_buf = max_buf;
};

void Destroy(struct Server *srvr) {
  free(srvr->buf);
};

// Listen on port. Returns true on success, and false otherwise.
bool Listen(struct Server *srvr, int port) {
  // Create socket
  srvr->listen_fd = -1;
  srvr->listen_fd = socket(AF_INET6, SOCK_STREAM, 0);
  if (srvr->listen_fd < 0) {
    perror("Socket create");
    return false;
  }

  // Create IPv6 sockaddr.
  struct sockaddr_in6 server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin6_family = AF_INET6;
  server_addr.sin6_port = htons(port);
  server_addr.sin6_addr = in6addr_any;

  int enable = 1;
  if (setsockopt(srvr->listen_fd, SOL_SOCKET, SO_REUSEADDR, &enable,
                 sizeof enable) < 0) {
      perror("setsockopt(SO_REUSEADDR) failed");
      return false;
  }

  // Bind to sockaddr.
  if (bind(srvr->listen_fd, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) != 0) {
      perror("Socket bind");
      return false;
  }

  // Listen
  if (listen(srvr->listen_fd, 20 /* backlog */) != 0) {
    perror("Socket listen");
    return false;
  }

  return true;
};

bool Serve(struct Server *srvr) {
  while (true) {
    // Accept connections.
    struct sockaddr_in6 client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(srvr->listen_fd, (struct sockaddr *)&client_addr,
                           &client_addr_len);

    // Get presentation form of client IPv6 IP.
    char paddr[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &client_addr.sin6_addr, paddr, sizeof(paddr));
    printf("[%s]:%d connected\n", paddr, ntohs(client_addr.sin6_port));

    // Read from socket.
    int nrecv = recv(client_fd, srvr->buf, srvr->max_buf, 0);
    if (nrecv == -1) {
      perror("Socket recv");
      return false;
    }
    bool ok = srvr->on_recv(srvr->buf, nrecv, client_fd);
    if (!ok) {
      return false;
    }

    // Close TCP connection.
    if (shutdown(client_fd, SHUT_RDWR) != 0) {
      perror("Socket shutdown");
      return false;
    }
    printf("[%s]:%d closed\n", paddr, ntohs(client_addr.sin6_port));
  }
};
