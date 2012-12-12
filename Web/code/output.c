#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

int main(){
  int i;
  int len;
  bool isEve;
  char str[97];

  len = atoi(getenv("CONTENT_LENGTH"));
  scanf("%s", str);

  /*if(len == 76) isEve = false;
	else */
  if(len == 96) isEve = true;
  else{
    printf("content-type: text/plain\n\n");
    printf("Error:1 %d %s", len, str);
    return -1;
  }

  FILE* fp;
  if((fp=fopen("./output.txt", "w")) == NULL){
    printf("content-type: text/plain\n\n");
    printf("Error:2");
    return -1;
  }

  int eq_flag = 0;
  int n = 0;
  for(i=0;i<len;i++){
	if(eq_flag == 0 || eq_flag == 2 || eq_flag == 4){
	  if(str[i] == '=') eq_flag ++;
      else continue;
	}else if(eq_flag == 1 || eq_flag == 3 || eq_flag == 5){
	  if(str[i] == '&'){
		if(!isEve && eq_flag==3) break;
		eq_flag ++;
		continue;
      }else if(str[i] != '0' && str[i] != '1' && str[i] != '2'){
		printf("content-type: text/plain\n\n");
		printf("Error:3");
		return -1;
      }
      fprintf(fp, "%c ", str[i]);
      if(++n % 20 == 0) fprintf(fp, "\n");
    }
  }
  fclose(fp);

  /*****************************************************************/

  struct stat stat_buf;
  while(true){
	if(stat("resultTmp", &stat_buf) == 0)
	  if(stat_buf.st_size != 0) break;
  }

  printf("content-type: text/plain\n\n");

  char copyBuf[128];
  FILE* index;
  FILE* result;
  if((index=fopen("./index.html", "r")) == NULL){
	printf("error");
	return -1;
  }
  if((result=fopen("./resultTmp", "r")) == NULL){
	printf("error");
	fclose(index);
	return -1;
  }

  while(strcmp(fgets(copyBuf, 128, index), "<body onload=\"setRandomAll();\">\n") != 0);
  while(fgets(copyBuf, 128, index) != NULL){
	if(strcmp(copyBuf, "<!--result-->\n") == 0) break;
	printf("%s", copyBuf);
  }

  while(fgets(copyBuf, 128, result) != NULL)
	printf("%s", copyBuf);

  while(strcmp(fgets(copyBuf, 128, index), "<!--/result-->\n") != 0);
  while(fgets(copyBuf, 128, index) != NULL){
	if(strcmp(copyBuf, "</body>\n") == 0) break;
	printf("%s", copyBuf);
  }

  fclose(index);
  fclose(result);

  remove("./resultTmp");

  return 0;
}
