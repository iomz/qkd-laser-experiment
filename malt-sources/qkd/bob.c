#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <memory.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/stat.h>

#define MAX_KEY_NUM 100

void usage() {
  printf("./a.out <addr>\n");
  return;
}

int main(int argc, char **argv) {
  if(argc != 2) {
    usage();
    return 0;
  }
  
  int i;

  int is_v6 = 0;
  for(i=0;i<strlen(argv[1]);i++){
	if(argv[1][i] == ':') is_v6 = 1;
  }
  uint32_t len, nlen;
  int port, sock, err;
  struct addrinfo hints, *res, *res0;
  char buf[512];

  port = 56789;
  
  memset(&hints, 0, sizeof(hints));
  if(is_v6)	hints.ai_family = AF_INET6;
  else		hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  err = getaddrinfo(argv[1], "56789", &hints, &res0);
  if(err) {
    fprintf(stderr, "getaddrinfo(): %s", gai_strerror(err));
    return -1;
  }
  
  sock = -1;
  for(res = res0; res; res = res->ai_next) {
    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sock < 0) 
      continue;
    
    if(connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
      close(sock);
      sock = -1;
      continue;
    }
    
    break;
  }
  
  if(sock < 0) {
    fprintf(stderr, "Error 1\n");
    return -1;
  }

  if(fgets(buf, 512, stdin) == NULL){
	fprintf(stderr, "Error 2\n");
	exit(0);
  }
  len = strlen(buf);
  nlen = htonl(len);
    
  write(sock, &nlen, 4);
  write(sock, buf, len);
    
  read(sock, &nlen, 4);
  len = ntohl(nlen);

  strcpy(buf, "");
  int cc = read(sock, buf, len);
  if(strncmp(buf, "NG", 2) == 0){
	fprintf(stderr, "Error 3\n");
	close(sock);
	return 0;
  }

  FILE* fp;
  char file_name[20];
  i = -1;
  do{
	sprintf(file_name, "../key/%d.key", ++i);
	fp = fopen(file_name, "r");
	if(i>MAX_KEY_NUM){
	  fprintf(stderr, "keyfile could not open.\n", file_name);
	  close(sock);
	  return 0;
	}
  }while(fp==NULL);

  len = strlen(file_name);
  nlen = htonl(len);
  write(sock, &nlen, 4);
  write(sock, file_name, len);
  strcpy(buf, "");
  read(sock, buf, 3);

  if(strncmp(buf, "NG", 2) == 0){
	fprintf(stderr, "Error 4\n");
	fclose(fp);
	remove(file_name);
	close(sock);
	return 0;
  }

  if(fgets(buf, 512, fp) == NULL){
	fprintf(stderr, "Error 5\n");
	fclose(fp);
	remove(file_name);
	close(sock);
	return 0;
  }

  fclose(fp);
  remove(file_name);

  printf("%s\n", buf);
  len = strlen(buf);
  nlen = htonl(len);
  write(sock, &nlen, 4);
  write(sock, buf, len);
  read(sock, buf, 3);

  if(strncmp(buf, "NG", 2) == 0){
	fprintf(stderr, "Error 6\n");
	close(sock);
	return 0;
  }

  struct timeval starttv;
  {
	struct timeval stv;
	char time[20];
	read(sock, time, 20);
	gettimeofday(&starttv, NULL);

	sscanf(time, "%ld.%d", (long*)&stv.tv_sec, (int*)&stv.tv_usec);
	
	if(starttv.tv_sec-stv.tv_sec < 2){
	  starttv.tv_sec += 7;
	}else{
	  starttv.tv_sec += (starttv.tv_sec - stv.tv_sec) + 7;
	}
	sprintf(time, "%ld.%d", (long)starttv.tv_sec, (int)starttv.tv_usec);
	write(sock, time, 20);
	sprintf(buf, "python qkd-mini.py bob %ld", (long)starttv.tv_sec);
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

	struct stat stat_buf;
	while(1){
	  if(stat("./bob.tmp", &stat_buf) == 0)
		if(stat_buf.st_size != 0) break;
	}
	fp = fopen("./bob.tmp", "r");

	for(i=0;i<10;i++){
	  fgets(buf, 10, fp);
	  sscanf(buf, "%c %d", &base[0], &bit);
	  write(sock, &base[0], 1);
	  read(sock, &base[1], 1);
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
		for(j=0;j<32;j++)
		  fprintf(key_fp, "%d", key[j]);
		fclose(key_fp);
		printf("made %s.\n", file_name);
	  }
	}
	fclose(fp);
	remove("./bob.tmp");
  }
  
  close(sock);
  
  return 0;
}
