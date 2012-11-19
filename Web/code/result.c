#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

void fprintBasis(FILE*, int);

int main(){
  FILE* result;
  FILE* index;
  int i, j;
  int len;
  char* str;

  if((result=fopen("result.html", "w")) == NULL){
	printf("content-type: text/html\n\n");
    printf("Error");
    return -1;
  }
  
  str = (char*)malloc(sizeof(char) * 28);

  len = atoi(getenv("CONTENT_LENGTH"));
  if(len != 27){
    fprintf(result, "Error");
	fclose(result);
    return -1;
  }

  scanf("%s", str);
  str += 7;
  
  fprintf(result, "<!DOCTYPE html>\n");
  fprintf(result, "<head>\n");
  fprintf(result, "\t<title>Loading...</title>\n");
  fprintf(result, "\t<meta http-equiv=\"Content-Type\" content=\"text/html;\" />\n");
  fprintf(result, "<meta http-equiv=\"Refresh\" content=\"0;URL=./index.html\" />\n");
  fprintf(result, "<link rel=\"stylesheet\" type=\"text/css\" media=\"screen\" href=\"load/load.css\" />\n");
  fprintf(result, "</head>\n");
  fprintf(result, "<body>\n");
  fprintf(result, "<div id=\"wrap\">\n");
  fprintf(result, "<img border=\"0\" src=\"load/load.gif\" />\n");
  fprintf(result, "</div>\n");
  fprintf(result, "</body>\n");

  fclose(result);

  if((result=fopen("./tmp", "w")) == NULL){
	printf("content-type: text/html\n\n");
    printf("Error");
    return -1;
  }
  if((index=fopen("./index.html", "r")) == NULL){
    fprintf(result, "Error");
	fclose(result);
    return -1;
  }
  char copyBuf[128];
  while(fgets(copyBuf, 128, index) != NULL){
	fprintf(result, "%s", copyBuf);
	if(strcmp(copyBuf, "<!--result-->\n") == 0) break;
  }
  
  /****************body******************/
  FILE* fp;
  int c;
  char outputText[4][64];
  bool isEve = true, isEavesdrop = false;
  if((fp=fopen("./output.txt", "r")) == NULL){
    fprintf(result, "Error");
	fclose(result);
	fclose(index);
    return -1;
  }
  for(i=0;i<4;i++)
	if(fgets(outputText[i], 64, fp) == NULL || outputText[i][0] == '2'){
	  isEve = false;
	  break;
	}
  fclose(fp);
  fprintf(result, "\t<table class=\"result\" border=\"1\" width=\"528px\" border=\"1\">\n\t\t<tr><td class=\"resultHead\" colspan=\"2\"></td>\n");
  for(i=0;i<20;i++) fprintf(result, "\t\t\t<td class=\"resultHead\" width=\"15px\"><font size=\"1\">%d</font></td>\n", i);
  fprintf(result, "\t\t</tr>\n");

  int rowspan;
  if(isEve) rowspan = 3;
  else      rowspan = 2;
  for(i=0;i<2+rowspan;i++){
	if(i==0)
	  fprintf(result, "\t\t<tr>\n\t\t\t<td class=\"resultHead\" rowspan=\"%d\">Basis</td>\n\n\t\t\t<td class=\"resultHead2\">Alice</td>", rowspan);
	else if(i==rowspan)
	  fprintf(result, "\n\n\t\t</tr><tr>\n\t\t\t<td class=\"resultHead\" rowspan=\"2\">Bit</td>\n\n\t\t\t<td class=\"resultHead2\">Alice</td>");
	else if(i==2)
	  fprintf(result, "\n\n\t\t</tr><tr>\n\n\t\t\t<td class=\"resultHead2\">Eve</td>");
	else
	  fprintf(result, "\n\n\t\t</tr><tr>\n\n\t\t\t<td class=\"resultHead2\">Bob</td>");

	for(j=0;j<20;j++){
	  if(outputText[1][j*2] == outputText[2][j*2]){
		if(outputText[0][j*2] == str[j])
		  fprintf(result, "\n\t\t\t<td class=\"resultBody\" style=\"background-color:#6495ed\">");
		else{
		  fprintf(result, "\n\t\t\t<td class=\"resultBody\" style=\"background-color:#dc143c\">");
		  isEavesdrop = true;
		}
	  }else fprintf(result, "\n\t\t\t<td class=\"resultBody\">");

	  if(i<=1)              fprintBasis(result, outputText[i+1][j*2]);
	  else if(i==rowspan)   fprintf(result, "%c", outputText[0][j*2]);
	  else if(i==rowspan+1) fprintf(result, "%c", str[j]);
	  else                  fprintBasis(result, outputText[3][j*2]);
	  fprintf(result, "</td>");
	}
  }

  fprintf(result, "\n\t\t</tr>\n\t</table>\n");

  if(isEve){
	if(isEavesdrop) fprintf(result, "\n<h3 style=\"color:#6495ed\">Detection Successful</h3>\n");
	else            fprintf(result, "\n<h3 style=\"color:#dc143c\">Detection Failed</h3>\n");
  }else{
	if(isEavesdrop) fprintf(result, "\n<h3 style=\"color:#dc143c\">Distribution Failed</h3>\n");
	else            fprintf(result, "\n<h3 style=\"color:#6495ed\">Distribution Successful</h3>\n");
  }
  /**************************************/
  while(strcmp(fgets(copyBuf, 128, index), "<!--/result-->\n") != 0);
  fprintf(result, "<!--/result-->\n");
  while(fgets(copyBuf, 128, index) != NULL){
	fprintf(result, "%s", copyBuf);
  }

  fclose(index);
  fclose(result);
  free(str-7);

  printf("content-type: text/html\n\n");
  errno = 0;
  if(rename("./tmp", "./index.html")!=0){
	printf("error: %d<br/>\n", errno);
  }

  printf("OK!");

  return 0;
}

void fprintBasis(FILE* fp, int c){
  if(c == '0'){
	fprintf(fp, "+");
  }else if(c == '1'){
	fprintf(fp, "×");
  }
}
