#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void myerror(const char* msg){
	perror(msg);
	exit(1);
}
