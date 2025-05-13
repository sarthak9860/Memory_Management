// Question

/** REQUIRED HEADER FILES **/

#include<stdio.h>
#include"../include/head.h"

/*
 * Name of the function:
 * Author: Sarthak Bosamiya
 * Created: 28-04-2025
 * Modified: 28-04-2025
*/

int main(){
	int num;
	printf("Enter the number of the exercise to be executed: ");
	scanf("%d", &num);

	switch(num){
		case 1:
			task1();
			break;
		case 2:
			task2();
			break;
		case 3:
			task3();
			break;
		default:
			printf("Enter the valid number\n");
			break;
	}
	return 0;
}
