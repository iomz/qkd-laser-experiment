#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>

void fprintBasis(FILE*, int);

int main(){
  printf("content-type: text/html\n\n");

  srand((unsigned)time(NULL));

  FILE* result;
  int i, j;
  int len;
  char* str;

  FILE* fp;
  int c;
  char outputText[4][64];
  bool isEve = true, isEavesdrop = false;
  if((fp=fopen("./output.txt", "r")) == NULL){
    printf("Error:8");
    return -1;
  }
  for(i=0;i<4;i++)
	if(fgets(outputText[i], 64, fp) == NULL || outputText[i][0] == '2'){
	  isEve = false;
	  break;
	}
  fclose(fp);
  
  if((result=fopen("resultTmp", "w")) == NULL){
	printf("content-type: text/html\n\n");
    printf("Error:4");
    return -1;
  }
  
  str = (char*)malloc(sizeof(char) * 28);

  /*len = atoi(getenv("CONTENT_LENGTH"));
  if(len != 27){
    fprintf(result, "Error:5");
	fclose(result);
    return -1;
  }

  scanf("%s", str);*/
  str += 7;

  /*************lie***************/
  for(i=0;i<20;i++){
	if(outputText[1][i*2]==outputText[2][i*2] && (outputText[1][i*2]==outputText[3][i*2] || !isEve)){
	  str[i] = outputText[0][i*2];
	}else{
	  char temp[2];
	  int tmp = rand() % 2;
	  sprintf(temp, "%d", tmp);
	  str[i] = temp[0];
	}
  }
  /*******************************/
  
  
  /****************body******************/
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
	if(isEavesdrop) fprintf(result, "\n\t<h3 style=\"color:#6495ed\">Detection Successful</h3>\n");
	else            fprintf(result, "\n\t<h3 style=\"color:#dc143c\">Detection Failed</h3>\n");
  }else{
	if(isEavesdrop) fprintf(result, "\n\t<h3 style=\"color:#dc143c\">Distribution Failed</h3>\n");
	else            fprintf(result, "\n\t<h3 style=\"color:#6495ed\">Distribution Successful</h3>\n");
  }
  /**************************************/

  fclose(result);

  free(str-7);

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
