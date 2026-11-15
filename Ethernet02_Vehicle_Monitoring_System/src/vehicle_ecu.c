#define _POSIX_C_SOURCE 200809L
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

static volatile sig_atomic_t running = 1;
static void stop_handler(int sig) { (void)sig; running = 0; }

static void usage(const char *p) {
    fprintf(stderr, "Usage: %s --protocol tcp|udp --host IPv4 --port PORT [--interval SECONDS]\n", p);
}
static int send_all(int fd, const char *buf, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, 0);
        if (n < 0) { if (errno == EINTR) continue; return -1; }
        if (n == 0) return -1;
        sent += (size_t)n;
    }
    return 0;
}
int main(int argc, char **argv) {
    const char *protocol = NULL, *host = NULL;
    int port = 0, interval = 1, sock = -1, is_tcp;
    struct sockaddr_in addr;
    unsigned long seq = 0;

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--protocol") && i+1 < argc) protocol = argv[++i];
        else if (!strcmp(argv[i], "--host") && i+1 < argc) host = argv[++i];
        else if (!strcmp(argv[i], "--port") && i+1 < argc) port = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--interval") && i+1 < argc) interval = atoi(argv[++i]);
        else { usage(argv[0]); return 1; }
    }
    if (!protocol || !host || port < 1 || port > 65535 || interval < 1 ||
        (strcmp(protocol,"tcp") && strcmp(protocol,"udp"))) { usage(argv[0]); return 1; }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET; addr.sin_port = htons((unsigned short)port);
    if (inet_pton(AF_INET, host, &addr.sin_addr) != 1) {
        fprintf(stderr, "Invalid IP address: %s\n", host); return 1;
    }
    is_tcp = !strcmp(protocol, "tcp");
    signal(SIGINT, stop_handler); signal(SIGTERM, stop_handler);
    signal(SIGPIPE, SIG_IGN);
    sock = socket(AF_INET, is_tcp ? SOCK_STREAM : SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket"); return 1; }
    if (is_tcp && connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect"); close(sock); return 1;
    }
    printf("Vehicle ECU sending %s telemetry to %s:%d (Ctrl+C to stop)\n", protocol, host, port);
    while (running) {
        char msg[256];
        int speed = (int)((seq * 7UL) % 120UL);
        int rpm = 800 + (int)(seq * 137UL);
        int steering = -60 + (int)((seq * 11UL) % 121UL);
        int n = snprintf(msg, sizeof(msg),
            "{\"seq\":%lu,\"speed_kmh\":%d,\"engine_rpm\":%d,\"steering_deg\":%d}\n",
            seq, speed, rpm, steering);
        if (n < 0 || (size_t)n >= sizeof(msg)) break;
        int rc = is_tcp ? send_all(sock, msg, (size_t)n) :
            (sendto(sock, msg, (size_t)n, 0, (struct sockaddr *)&addr, sizeof(addr)) < 0 ? -1 : 0);
        if (rc < 0) { perror(is_tcp ? "send" : "sendto"); break; }
        printf("Sent: %s", msg); fflush(stdout);
        ++seq;
        for (int s = 0; s < interval && running; ++s) sleep(1);
    }
    close(sock);
    puts("Vehicle ECU stopped.");
    return 0;
}
