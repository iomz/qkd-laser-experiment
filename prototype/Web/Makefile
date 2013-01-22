all: output.cgi result.cgi rmTmp

output.cgi:	./code/output.c
	gcc ./code/output.c -o ./output.cgi
result.cgi: ./code/result.c
	gcc ./code/result.c -o ./result.cgi
rmTmp: ./code/rmTmp.c
	gcc ./code/rmTmp.c -o ./rmTmp