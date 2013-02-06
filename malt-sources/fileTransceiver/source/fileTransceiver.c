#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <memory.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>

#include "blowfish.h"

#define KEY_LEN 32
#define MAX_KEY_NUM 100

int makeClientSocket(char*,char*);
int makeServerSocket(char*);
void swap(char*,char*,int);
void receiveImage(FILE*,int,BLOWFISH_CTX*);
void sendImage(FILE*,int,BLOWFISH_CTX*);
void strCpyOwn(char*,int);

void usage() {
  printf("./fileTransceiver <addr>\n");
  return;
}

void printCommandList(){
  printf("*********** Command List ***********\n");
  printf("s <file name> : send <file name>\n");
  printf("r <file name> : request <file name>\n");
  printf("exit          : quit\n");
  printf("?             : print command list\n");
  printf("************************************\n");
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
  int sock_s, sock_c, acceptfd;
  socklen_t clen;
  struct sockaddr client;
  char port[2][7] = {"65456","56765"};
  char buf[512];
  BLOWFISH_CTX ctx;
  
  signal(SIGCHLD, sfunc);
  
  if(argc != 2) {
    usage();
    return 0;
  }

  if((sock_c=makeClientSocket(argv[1],port[0])) != -1){
	if(sock_c == -2) return -1;
	printf("Connect!\n\n");
	swap(port[0], port[1], 7);
  }

  pid = fork();

  if(pid == 0){
	uint32_t nlen, len;
	char* file_name;
	FILE *fp, *kfp;
	char key_file_name[20], open_mode[3];
	int is_sendmode;

	sock_s = makeServerSocket(port[0]);

	listen(sock_s, 5);
	clen = sizeof(client);
	memset(&client, 0, clen);

	acceptfd = accept(sock_s, &client, &clen);
	close(sock_s);
	write(acceptfd, "OK\0", 3);

	while(1){
	  read(acceptfd, &is_sendmode, sizeof(int));
	  is_sendmode = !is_sendmode;
	  if(is_sendmode)	strcpy(open_mode, "rb");
	  else				strcpy(open_mode, "wb");
	  read(acceptfd, &nlen, 4);
	  len = ntohl(nlen);
	  file_name = malloc(sizeof(char)*(len+1));
	  memset(file_name, 0, len+1);
	  read(acceptfd, file_name, len);
	  if(file_name[len-1] == '\n')
		file_name[len-1] = '\0';
	  
	  if(strncmp(file_name, "exit", 4) == 0 && strlen(buf) == 4 || file_name[0]=='\0') break;
	  if((fp=fopen(file_name, open_mode)) == NULL){
		printf("%s could not open.\n", file_name);
		fprintf(stderr, "\n:");
		write(acceptfd, "NG\0", 3);
		remove(file_name);
		continue;
	  }else	write(acceptfd, "OK\0", 3);

	  free(file_name);

	  while(1){
		read(acceptfd, &nlen, 4);
		len = ntohl(nlen);
		memset(key_file_name, 0, 10);
		read(acceptfd, key_file_name, len);

		if(strncmp(key_file_name, "NG", 2) == 0)	break;

		if((kfp=fopen(key_file_name, "r")) == NULL){
		  fprintf(stderr, "could not open %s.\n", key_file_name);
		  write(acceptfd, "NG\0", 3);
		  remove(key_file_name);
		}else{
		  write(acceptfd, "OK\0", 3);
		  break;
		}
	  }
	  if(strncmp(key_file_name, "NG", 2) == 0)	break;

	  fgets(buf, KEY_LEN, kfp);
	  buf[KEY_LEN] = '\0';
  	  Blowfish_Init(&ctx, buf, KEY_LEN);
	  fclose(kfp);
	  remove(key_file_name);

	  if(is_sendmode)	sendImage(fp, acceptfd, &ctx);
	  else				receiveImage(fp, acceptfd, &ctx);

	  fclose(fp);
	  if(is_sendmode)	fprintf(stderr, "sent %s.\n\n:", file_name);
	  else				fprintf(stderr, "receive %s.\n\n:", file_name);
	}
	close(acceptfd);
	printf("disconnected.\n");
	kill(getppid(), SIGTERM);
	exit(0);
  }else{
	uint32_t nlen, len;
	FILE *fp, *kfp;
	char key_file_name[20], open_mode[3];
	int i;
	int is_sendmode;
	
	if(sock_c == -1){
	  printf("Waiting...\n");
	  while((sock_c=makeClientSocket(argv[1], port[1])) == -1);
	  printf("Connect!\n\n");
	}

	printCommandList();

	while(1){
	  memset(buf, 0, sizeof(buf));
	  printf("\n:");
	  if(fgets(buf, 512, stdin) == NULL){
		printf("could not read stdin.\n");
		break;
	  }

	  if(strncmp(buf, "r ", 2) == 0){
		is_sendmode = 0;
		strCpyOwn(buf, 2);
		strcpy(open_mode, "wb");
	  }else if(strncmp(buf, "s ", 2) == 0){
		is_sendmode = 1;
		strCpyOwn(buf, 2);
		strcpy(open_mode, "rb");
	  }else if(strcmp(buf, "exit\n") != 0){
		printCommandList();
		continue;
	  }

	  for(i=0;i<512;i++){
		if(buf[i] == '\n'){
		  buf[i] = '\0';
		  break;
		}
	  }
	  char name[512];
	  char* cp = &buf[i];
	  while(*(cp-1) != '/'){
		cp --;
		if(cp == &buf[0])	break;
	  }
	  strcpy(name, cp);
	  if(strncmp(buf, "exit", 4) == 0 && strlen(buf) == 4) break;
	  if(is_sendmode){
		if((fp=fopen(buf,open_mode)) == NULL){
		  printf("file could not open.\n");
		  continue;
		}
		len = strlen(name);
	  }else{
		if((fp=fopen(name,open_mode)) == NULL){
		  printf("file could not open.\n");
		  continue;
		}
		len = strlen(buf);
	  }

	  write(sock_c, &is_sendmode, sizeof(int));

	  nlen = htonl(len);
	  write(sock_c, &nlen, 4);
	  if(is_sendmode)	write(sock_c, name, len);
	  else				write(sock_c, buf, len);
	  read(sock_c, buf, 3);
	  if(strncmp(buf, "NG", 2) == 0){
		printf("file could not open on server.\n");
		fclose(fp);
		remove(name);
		continue;
	  }

	  while(1){
		i = -1;
		do{
		  sprintf(key_file_name, "../key/%d.key", ++i);
		  kfp = fopen(key_file_name, "r");
		  if(i>MAX_KEY_NUM){
			fprintf(stderr, "could not open keyfile.\n");
			strcpy(key_file_name, "NG");
			break;
		  }
		}while(kfp==NULL);

		len = strlen(key_file_name);
		nlen = htonl(len);
		write(sock_c, &nlen, 4);
		write(sock_c, key_file_name, len);

		if(strcmp(key_file_name, "NG") == 0) break;
		read(sock_c, buf, 3);
		
		if(strncmp(buf, "NG", 2) == 0){
		  fprintf(stderr, "could not open %s on server.\n", key_file_name);
		  fclose(kfp);
		  remove(key_file_name);
		}else break;
	  }
	  if(strcmp(key_file_name, "NG") == 0) break;

	  fgets(buf, KEY_LEN, kfp);
	  buf[KEY_LEN] = '\0';
  	  Blowfish_Init(&ctx, buf, KEY_LEN);
	  fclose(kfp);
	  remove(key_file_name);

	  if(is_sendmode)	sendImage(fp, sock_c, &ctx);
	  else				receiveImage(fp, sock_c, &ctx);

	  fclose(fp);
	  if(is_sendmode)	printf("sent %s.\n", name);
	  else				printf("receive %s.\n", name);
	}
	close(sock_c);
  }
  printf("disconnected.\n");
  kill(pid, SIGTERM);
  return 0;
}

int makeServerSocket(char* port){
  struct addrinfo hints, *res, *res0;
  int sock, err;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_socktype = SOCK_STREAM;
  err = getaddrinfo(NULL, port, &hints, &res0);
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
  
  if(sock < 0)
	return -1;

  return sock;
}

int makeClientSocket(char* addr, char* port){
  int i;
  int is_v6 = 0;
  for(i=0;i<strlen(addr);i++)
	if(addr[i] == ':') is_v6 = 1;
  int sock, err;
  struct addrinfo hints, *res, *res0;
  char buf[3];

  memset(&hints, 0, sizeof(hints));
  if(is_v6)	hints.ai_family = AF_INET6;
  else		hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  err = getaddrinfo(addr, port, &hints, &res0);
  if(err) {
    fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(err));
    return -2;
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
    return -1;
  }

  read(sock, buf, 3);

  if(strncmp(buf, "OK", 2) == 0)	return sock;
  return -1;
}

void swap(char* a, char* b, int len){
  char* tmp;
  tmp = (char*)malloc(sizeof(char)*len);
  strcpy(tmp, a);
  strcpy(a, b);
  strcpy(b, tmp);
  free(tmp);
}

void receiveImage(FILE* fp, int sock, BLOWFISH_CTX *ctx){
  unsigned long L, R;
  do{
	L = 0; R = 0;
	read(sock, &L, sizeof(unsigned long));
	read(sock, &R, sizeof(unsigned long));

	Blowfish_Decrypt(ctx, &L, &R);

	fwrite(&L, sizeof(unsigned long), 1, fp);
	fwrite(&R, sizeof(unsigned long), 1, fp);
  }while(L != 0);
  return;
}

void sendImage(FILE* fp, int sock, BLOWFISH_CTX *ctx){
  unsigned long L, R;
  int flag;
  do{
	flag = 1;
	L = 0; R = 0;
	fread(&L, sizeof(unsigned long), 1, fp);
	fread(&R, sizeof(unsigned long), 1, fp);
	if(L == 0) flag = 0;
	
	Blowfish_Encrypt(ctx, &L, &R);
	
	write(sock, &L, sizeof(unsigned long));
	write(sock, &R, sizeof(unsigned long));
  }while(flag);
  return;
}

void strCpyOwn(char* str, int n){
  int i, len=strlen(str);
  for(i=n;i<len;i++) str[i-n] = str[i];
  str[len-n] = '\0';
  return;
}
