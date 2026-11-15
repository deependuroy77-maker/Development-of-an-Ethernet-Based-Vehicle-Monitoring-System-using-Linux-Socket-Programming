#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static volatile sig_atomic_t running = 1;
static void stop_handler(int sig) { (void)sig; running = 0; }
static void usage(const char *p) {
    fprintf(stderr, "Usage: %s --protocol tcp|udp --bind IPv4 --port PORT\n", p);
}
static void print_records(char *buf, size_t *used) {
    size_t start = 0;
    for (size_t i = 0; i < *used; ++i) {
        if (buf[i] == '\n') {
            buf[i] = '\0';
            if (buf[start]) printf("Received telemetry: %s\n", buf + start);
            start = i + 1;
        }
    }
    if (start) { memmove(buf, buf + start, *used - start); *used -= start; }
}
int main(int argc, char **argv) {
    const char *protocol = NULL, *bind_ip = NULL;
    int port = 0, server = -1, client = -1, is_tcp;
    struct sockaddr_in addr;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--protocol") && i+1 < argc) protocol = argv[++i];
        else if (!strcmp(argv[i], "--bind") && i+1 < argc) bind_ip = argv[++i];
        else if (!strcmp(argv[i], "--port") && i+1 < argc) port = atoi(argv[++i]);
        else { usage(argv[0]); return 1; }
    }
    if (!protocol || !bind_ip || port < 1 || port > 65535 ||
        (strcmp(protocol,"tcp") && strcmp(protocol,"udp"))) { usage(argv[0]); return 1; }
    memset(&addr, 0, sizeof(addr)); addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)port);
    if (inet_pton(AF_INET, bind_ip, &addr.sin_addr) != 1) {
        fprintf(stderr, "Invalid bind IP address: %s\n", bind_ip); return 1;
    }
    is_tcp = !strcmp(protocol, "tcp");
    signal(SIGINT, stop_handler); signal(SIGTERM, stop_handler);
    server = socket(AF_INET, is_tcp ? SOCK_STREAM : SOCK_DGRAM, 0);
    if (server < 0) { perror("socket"); return 1; }
    int yes = 1; setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (bind(server, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(server); return 1;
    }
    if (is_tcp) {
        if (listen(server, 5) < 0) { perror("listen"); close(server); return 1; }
        printf("Dashboard ECU listening via tcp on %s:%d\n", bind_ip, port); fflush(stdout);
        while (running) {
            client = accept(server, NULL, NULL);
            if (client < 0) { if (errno == EINTR) continue; perror("accept"); break; }
            char buf[4096]; size_t used = 0;
            while (running) {
                ssize_t n = recv(client, buf + used, sizeof(buf) - used - 1, 0);
                if (n < 0) { if (errno == EINTR) continue; perror("recv"); break; }
                if (n == 0) break;
                used += (size_t)n; buf[used] = '\0'; print_records(buf, &used);
                if (used == sizeof(buf)-1) used = 0;
                fflush(stdout);
            }
            close(client); client = -1;
        }
    } else {
        printf("Dashboard ECU listening via udp on %s:%d\n", bind_ip, port); fflush(stdout);
        while (running) {
            char buf[4096];
            ssize_t n = recvfrom(server, buf, sizeof(buf)-1, 0, NULL, NULL);
            if (n < 0) { if (errno == EINTR) continue; perror("recvfrom"); break; }
            buf[n] = '\0';
            char *nl = strchr(buf, '\n'); if (nl) *nl = '\0';
            printf("Received telemetry: %s\n", buf); fflush(stdout);
        }
    }
    if (client >= 0) close(client);
    close(server);
    puts("Dashboard ECU stopped.");
    return 0;
}
