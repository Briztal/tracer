//
// Created by root on 11/9/18.
//


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define T ((float) 0.1)

#define N ((size_t) 1000)


#define A ((float) 10)
#define B ((float) 230)
#define C ((float) 15)
#define D ((float) 3.14)


int main() {
	
	printf("BITE LOL");

}


float get_y1(float ynext, float y) {
	
	return (ynext - y) / T;
	
}


float get_y2(float y, float y1) {
	
	return -B / A * y1 - C / A * y + D + get_y1(2, 3);
	
}


float get_ynext(float y, float y1, float y2) {
	
	return y + T * y1 + (T * T / 2) * y2;
	
}


struct triplet {



};





