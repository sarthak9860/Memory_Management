// Question

/** REQUIRED HEADER FILES **/

#include<stdio.h>
#include<unistd.h>
#include"../include/head.h"

/*
 * Name of the function:
 * Author: Sarthak Bosamiya
 * Created: 28-04-2025
 * Modified: 28-04-2025
*/

void task1(){
	printf("Process ID: %d\n", getpid());
	printf("Program is running......... Press Enter key to exit\n");
	//sleep(200);
	getchar();
	getchar();
	printf("Program Finished\n");
}
