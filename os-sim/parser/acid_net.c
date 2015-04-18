/* char to -> acid_long */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

unsigned long int
acidIP2long (char *ip)
{

  struct in_addr inp;
  unsigned long int acid_convert_value = (u_long) 4294967296;
  unsigned long int dir;

  inet_aton (ip, &inp);
  dir = htonl (inp.s_addr);
  if (dir < 0)
    dir = acid_convert_value - abs (dir);

  return dir;
}
