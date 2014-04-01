#include <ifaddrs.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

/**
 *
 * ifconf 和 ifreq 的结构定义贴在这里
 *
 * struct ifconf {
 *    int ifc_len;
 *    union {
 *      void *ifcu_buf;
 *      struct ifreq *ifcu_req;
 *    } ifc_ifcu;
 * };
 *
 * #define ifc_buf ifc_ifcu.ifcu_buf  // buffer address 
 * #define ifc_req ifc_ifcu.ifcu_req  // array of structures returned
 *
 * #define IFNAMESIZE   16
 *
 * struct ifreq {
 *    char ifr_name[IFNAMESIZE];   // interface name, e.g., "eth0", "wlan0"
 *    union {
 *      struct sockaddr ifru_addr;
 *      struct sockaddr ifru_dstaddr;
 *      struct sockaddr ifru_broadaddr;
 *      short ifru_flags;
 *      int   ifru_metric;
 *      void *  ifru_data;
 *    } ifr_ifru;
 * }
 *
 * #define ifr_addr         ifr_ifru.ifru_addr
 * #define ifr_dstaddr      ifr_ifru.ifru_dstaddr
 * #define ifr_broadaddr    ifr_ifru.ifru_broadaddr
 * #define ifr_flags        ifr_ifru.ifru_flags
 * #define ifr_metric       ifr_ifru.ifru_metric
 * #define ifr_data         ifr_ifru.ifru_data
 */
int main(int argc, char *argv[]) {
  struct ifconf ifc = {0, 0};
  struct ifreq ifq[20] = {0};
  struct ifreq ifqmac[20] = {0};

  ifc.ifc_len = sizeof(ifq);
  ifc.ifc_ifcu.ifcu_buf = (char *)ifq;

  int sock = socket(PF_INET, SOCK_DGRAM, 0);
  ioctl(sock, SIOCGIFCONF, &ifc);

  int ifcount = ifc.ifc_len / sizeof(struct ifreq);
  for(int i = 0 ; i < ifcount ; ++i) {
    printf("interface name: %s\nfamily: %d\naddr: %s\n", 
        ifc.ifc_req[i].ifr_name,
        ifc.ifc_req[i].ifr_ifru.ifru_addr.sa_family,
        inet_ntoa(((struct sockaddr_in *)&(ifc.ifc_req[i].ifr_addr))->sin_addr)
        );

    strcpy(ifqmac[i].ifr_name, ifc.ifc_req[i].ifr_name);
    ioctl(sock, SIOCGIFHWADDR, &ifqmac[i]);

    char hwaddr[13] = {0};
    printf("mac address: ");
    for(int j = 0 ; j < 6 ; ++j) {
      sprintf(hwaddr + 2 * j, "%02X", (unsigned char)ifqmac[i].ifr_addr.sa_data[j]);
    }
    hwaddr[12] = '\0';

    printf("%s\n", hwaddr);
  }

  return 0;
}
