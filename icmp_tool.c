// includes
#include "icmp_tools.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

static uint16_t compute_icmp_checksum(const void *buff, int length) {
    uint32_t sum;
    const uint16_t *ptr = buff;
    assert (length % 2 == 0);
    for (sum = 0; length > 0; length -= 2)
        sum += *ptr++;
    sum = (sum >> 16) + (sum & 0xffff);
    return (uint16_t)(~(sum + (sum >> 16)));
}

void send_single_icmp(int sockfd, const char *ip, uint16_t id, uint16_t sequence, int ttl) {
    struct icmphdr icmp_header;
    icmp_header.type = ICMP_ECHO;
    icmp_header.code = 0;
    icmp_header.un.echo.id = id;
    icmp_header.un.echo.sequence = sequence;
    icmp_header.checksum = 0;
    icmp_header.checksum = compute_icmp_checksum((uint16_t *)&icmp_header, sizeof(icmp_header));

    struct sockaddr_in recipient;
    bzero(&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;
    int inet_pton_ret = inet_pton(AF_INET, ip, &recipient.sin_addr);
    assert(inet_pton_ret == 1);

    int setsockopt_ret = setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
    if (setsockopt_ret != 0) {
        handle_error("setsockopt");
    }

    ssize_t bytes_sent = sendto(sockfd, &icmp_header, sizeof(icmp_header), 0,
                               (struct sockaddr *)&recipient, sizeof(recipient));

    if (bytes_sent < 0) {
        handle_error("sendto");
    }
}

static int time_passed(int packets_received, struct timeval *current_time,
                       struct timeval *end_time, int nqueries) {
    if (packets_received >= nqueries || timercmp(current_time, end_time, >)) {
        return 1;
    }

    return 0;
}
