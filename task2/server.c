#define _GNU_SOURCE
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 65432 // 2828 --> any num between 1024 and 65535



void handle_request(int nfd)
{
   FILE *network = fdopen(nfd, "r+"); // r --> r+ so that it can read + write
   char *line = NULL;
   size_t size;
   ssize_t num;

   if (network == NULL)
   {
      perror("fdopen");
      close(nfd);
      return;
   }

   while ((num = getline(&line, &size, network)) >= 0)
   {
      //printf("%s", line);
      write(nfd, line, num);
      ////send_request(nfd);
   }

   free(line);
   fclose(network);
}

void run_service(int fd)
{
   while (1)
   {
      int nfd = accept_connection(fd);
      if (nfd != -1)
      {
         printf("Connection established\n");
         handle_request(nfd);
         printf("Connection closed\n");
      }
   }
}

int main(void)
{
   int fd = create_service(PORT);

   if (fd == -1)
   {
      perror(0);
      exit(1);
   }

   printf("listening on port: %d\n", PORT);
   run_service(fd);
   close(fd);

   return 0;
}
