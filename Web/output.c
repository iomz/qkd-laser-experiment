#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(){
  FILE* result;
  if((result=fopen("result.html", "w")) == NULL){
	printf("content-type: text/html\n\n");
    printf("Error");
    return -1;
  }
  fprintf(result, "<!DOCTYPE html>\n");
  fprintf(result, "<head>\n");
  fprintf(result, "<title>Loading...</title>\n");
  fprintf(result, "<meta http-equiv=\"Content-Type\" content=\"text/html;\" />\n");
  fprintf(result, "<meta http-equiv=\"Refresh\" content=\"5;\" />\n");
  fprintf(result, "<link rel=\"stylesheet\" type=\"text/css\" media=\"screen\" href=\"load/load.css\" />\n");
  fprintf(result, "</head>\n");
  fprintf(result, "<body onload=\"setTimeout('location.reload()', 5000)\">\n");
  fprintf(result, "<div id=\"wrap\">\n");
  fprintf(result, "<img border=\"0\" src=\"load/load.gif\" />\n");
  fprintf(result, "</div>\n");
  fprintf(result, "</body>\n");
  fclose(result);
  
  int i;
  int len;
  bool isEve;
  char str[97];

  len = atoi(getenv("CONTENT_LENGTH"));
  scanf("%s", str);
  
  if(len == 76) isEve = false;
  else if(len == 96) isEve = true;
  else{
    printf("content-type: text/html\n\n");
    printf("Error", str);
    return -1;
  }

  FILE* fp;
  if((fp=fopen("./output.txt", "w")) == NULL){
    printf("content-type: text/html\n\n");
    printf("Error");
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
      }else if(str[i] != '0' && str[i] != '1'){
		printf("content-type: text/html\n\n");
		printf("Error");
		return -1;
      }
      fprintf(fp, "%c ", str[i]);
      if(++n % 20 == 0) fprintf(fp, "\n");
    }
  }
  fclose(fp);

  char buf[64];
  fp = fopen("./output.txt", "r");
  printf("content-type: text/html\n\n");
  printf("<!DOCTYPE html>\n");
  printf("<head>\n");
  printf("<title>Loading...</title>\n");
  printf("<meta http-equiv=\"Content-Type\" content=\"text/html;\" />\n");
  printf("<meta http-equiv=\"Refresh\" content=\"0;URL=./result.html\" />\n");
  printf("<link rel=\"stylesheet\" type=\"text/css\" media=\"screen\" href=\"load/load.css\" />\n");
  printf("</head>\n");
  printf("<body onload=\"setTimeout('location.reload()', 5000)\">\n");
  while(fgets(buf, sizeof(buf), fp) != NULL)
    printf("%s<br/>", buf);
  printf("<div id=\"wrap\">\n");
  printf("<img border=\"0\" src=\"load/load.gif\" />\n");
  printf("</div>\n");
  printf("</body>\n");
  fclose(fp);

  return 0;
}
