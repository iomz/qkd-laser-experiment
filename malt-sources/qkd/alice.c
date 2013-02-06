#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <sys/stat.h>

#define PASS "malt\n"
#define MAX_KEY_NUM 100

void usage() {
  printf("./server\n");
  return;
}

void sfunc(int sig){
  int ret = 0, status;

  do{
    ret = waitpid(-1, &status, WNOHANG);
  }while(ret > 0);
}

int main(int argc, char **argv) {
  pid_t pid;
  uint32_t nlen, len;
  int port, sock, err, acceptfd;
  socklen_t clen;
  struct addrinfo hints, *res, *res0;
  struct sockaddr client;
  char buf[512];

  signal(SIGCHLD, sfunc);
  
  if(argc != 1) {
    usage();
    return 0;
  }

  port = 56789;
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_socktype = SOCK_STREAM;
  err = getaddrinfo(NULL, "56789", &hints, &res0);
  if(err) {
    fprintf(stderr, "getaddrinfo(): %s", gai_strerror(err));
    return -1;
  }
  
  sock = -1;
  for(res = res0; res; res = res->ai_next) {
    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sock < 0) 
      continue;

    bind(sock, res->ai_addr, res->ai_addrlen);
    
    break;
  }
  
  if(sock < 0) {
    fprintf(stderr, "error");
    return -1;
  }

  listen(sock, 5);
  clen = sizeof(client);
  memset(&client, 0, clen);

  acceptfd = accept(sock, &client, &clen);
    
  close(sock);

  read(acceptfd, &nlen, 4);
  len = ntohl(nlen);
	
  while(len > 0){
	int cc = read(acceptfd, buf, sizeof(buf));
	len -= cc;
  }
      
  if(strcmp(buf, PASS) == 0){
	strcpy(buf, "OK");
		
  }else{
	strcpy(buf, "NG");
  }
	  
  len = strlen(buf);
  nlen = htonl(len);
  write(acceptfd, &nlen, 4);
  write(acceptfd, buf, len);

  char file_name[20] = "";
  read(acceptfd, &nlen, 4);
  len = ntohl(nlen);
  read(acceptfd, file_name, len);
  FILE* fp;
  if((fp=fopen(file_name, "r")) == NULL){
	fprintf(stderr, "could not open %s.\n", file_name);
	write(acceptfd, "NG\0", 3);
	remove(file_name);
	exit(0);
  }else{
	write(acceptfd, "OK\0", 3);
  }

  memset(buf, 0, sizeof(buf));
  read(acceptfd, &nlen, 4);
  len = ntohl(nlen);
  read(acceptfd, buf, len);
  {
	char key[512];
	fgets(key, 512, fp);
	fclose(fp);
	remove(file_name);

	if(strcmp(key, buf) != 0){
	  fprintf(stderr, "%s\n%s\n", key, buf);
	  write(acceptfd, "NG\0", 3);
	  exit(0);
	}else{
	  write(acceptfd, "OK\0", 3);
	}
	printf("%s\n", key);
  }

  struct timeval tv;
  {
	char time[20];
	gettimeofday(&tv, NULL);
	printf("%ld.%d\n", (long)tv.tv_sec, (int)tv.tv_usec);
	sprintf(time, "%ld.%d", (long)tv.tv_sec, (int)tv.tv_usec);
	write(acceptfd, time, 20);
	read(acceptfd, time, 20);
	sscanf(time, "%ld.%d", (long*)&tv.tv_sec, (int*)&tv.tv_usec);
	sprintf(buf, "python qkd-mini.py alice %ld", (long)tv.tv_sec);
	printf("%s\n", buf);
	if(fork() == 0){
	  system(buf);
	  exit(0);
	}
  }

  int next = 0;
  while(1){
	char base[2];
	int bit;
	int key[32];
	FILE* key_fp;
	int i;

	struct stat stat_buf;
	while(1){
	  if(stat("./alice.tmp", &stat_buf) == 0)
		if(stat_buf.st_size != 0) break;
	}
	fp = fopen("./alice.tmp", "r");

	for(i=0;i<10;i++){
	  fgets(buf, 10, fp);
	  sscanf(buf, "%c %d", &base[0], &bit);
	  read(acceptfd, &base[1], 1);
	  write(acceptfd, &base[0], 1);
	  if(base[0] == base[1]) key[next++] = bit;
	  fprintf(stderr, "%c,%c %d\n", base[0], base[1], bit);

	  if(next == 32){
		next = 0;
		int j;
		for(j=0;j<MAX_KEY_NUM;j++){
		  sprintf(file_name, "../key/%d.key", j);
		  if(access(file_name, F_OK) == -1){
			if((key_fp = fopen(file_name, "w")) == NULL){
			  fprintf(stderr, "%s could not open\n", file_name);
			  return -1;
			}
			break;
		  }
		}
		for(j=0;j<20;j++)
		  fprintf(key_fp, "%d", key[j]);
		fclose(key_fp);
		printf("made %s.\n", file_name);
	  }
	}
	fclose(fp);
	remove("./alice.tmp");
  }
  
  close(acceptfd);
  
  return 0;
}
