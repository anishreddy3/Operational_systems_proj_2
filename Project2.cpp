// Project2.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Operational Systems : Project 2 : Utilized semaphores and mutexes to create a bounded buffer problem using multiple threads to address the classic producer consumer algorithm.

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "time_functions.h"
#include "pthread.h"
#include "semaphore.h"
#include "sched.h"
#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
char in_path[200];
char out_path[200];
#endif
#ifdef _WIN32  //Windows
       char in_path[]="C:\\temp\\coursein\\p2-in.txt";
       char out_path[]="C:\\temp\\courseout\\p2-out.txt";  // change to "x:fileio\\courseout\\p1-out.txt" where x is the drive to use if you have one
#endif
//char in_path[200];
//char out_path[200];
void setpath();
// These 2 lines belong BEFORE main, so the names will be global
       FILE* infile;
       FILE* outfile;

void setpath()
{
#ifdef _WIN32  //Windows
       char in_path[]="C:\\temp\\coursein\\p2-in.txt";
       char out_path[]="C:\\temp\\courseout\\p2-out.txt";
#else //Linux
       // For Linux, the "home" directory actually contains the current userid
       // so we have to get that name from the system and put it in front of the path.
       // Many Linux shells will only expand the "~" if it is used from the command line
       
        strcpy(out_path,"/fileio/courseout/p2-out.txt");
       // outpath is NOT in your home directory!!!
             const char *homedir;
             homedir = getenv("HOME");
             if (homedir!= NULL)
                    homedir = getpwuid(getuid())->pw_dir;
             // get the actual home directory for the current user
             strcpy(in_path,homedir);
             // the outfile goes to a separate drive, NOT in your home directory
             // now add on the common part of the path
             strcat(in_path,"/temp/coursein/p2-in.txt");
#endif
}
using std::string;
#define BUFFER_SIZE 10
typedef string Bufferitem;
Bufferitem buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
sem_t full;
sem_t empty;
int counter;
int breakCondition = 0;



void insertline(char* item) {

	if (counter < BUFFER_SIZE) {
		buffer[counter] = item;
		counter++;
		return;
	}
	else
		printf("print error inserting line");
	   
}

char* removeline() {

	if (counter > 0) {
		//std::string itemRem;
		//itemRem = buffer[counter - 1];
		char *temp = (char*)malloc(sizeof(char)*1000);
		strcpy(temp, buffer[counter-1].c_str());
		counter--;
		return temp;
	}
	else
		exit(0);
}

 void *producer(void *)
{
	 
	char newline[1000];
	double wall_clock_diff;
	double nano_diff;
	infile = fopen(in_path, "r");
	start_timing();
	start_nanotime();
	while (fgets(newline, 1000, infile) != NULL)
	{
		
		sem_wait(&full);
		pthread_mutex_lock(&mutex);
		insertline(newline);
		pthread_mutex_unlock(&mutex);
		sem_post(&empty);
	}
	fclose(infile);
	stop_timing();
	nano_diff = get_nanodiff();
	wall_clock_diff = get_wall_clock_diff();
	printf("time taken to read by the producer in cpu time is: %f \n", nano_diff);
	printf("time taken to read by the producer in wall_clock is: %f \n", wall_clock_diff);
	breakCondition = 1;
	return 0;
	
	
}

void *consumer(void *)
 {
	start_timing();
	start_nanotime();
	double wall_clock_diff;
	double nano_diff;
	while (1) {
		
		
		outfile = fopen(out_path, "a");
		
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);
		char *itemfile = (char*)malloc(sizeof(char)*1000);
		strcpy(itemfile, removeline());

		fprintf(outfile,itemfile);
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
		
		fclose(outfile);
		
		if (breakCondition == 1) {
			break;
		}
	}
	stop_timing();
	nano_diff = get_nanodiff();
	wall_clock_diff = get_wall_clock_diff();
	printf("time taken to read by the consumer in cpu time is: %f \n", nano_diff);
	printf("time taken to read by the consumer in wall_clock is: %f \n", wall_clock_diff);
	
	
	return 0;
	
}
void comparefunc()
{
	
	int ch1, ch2;
	infile = fopen(in_path, "r");
	outfile = fopen(out_path, "r");
	if (infile == NULL) {
		printf("Cannot open producer file for reading");
		exit(1);
	}
	else if (outfile == NULL) {
		printf("Cannot open consumer file for reading");
		exit(1);
	}
	else {
		ch1 = getc(infile);
		ch2 = getc(outfile);


	}

	while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2)) {
		ch1 = getc(infile);
		ch2 = getc(outfile);
	}
}
int main(){	

	
	   setpath();    
printf("in path is %s",in_path);
printf("out path is %s",out_path);
      
	 pthread_t tid1;      
	 pthread_t tid2;		
	 
	
	 pthread_attr_t attr;
	
	 pthread_mutex_init(&mutex, NULL);
	 sem_init(&full,0,BUFFER_SIZE);
	 sem_init(&empty,0, 0);
	 counter = 0;

	 fflush(stdout);

	 
									
	 pthread_attr_init(&attr); 
	 pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);  
	 

	 pthread_create(&tid1, NULL, producer, NULL);
	 pthread_create(&tid2, NULL, consumer, NULL);


	 
	 pthread_join(tid1, NULL);
	 pthread_join(tid2, NULL);
	 /*pthread_mutex_destroy(&mutex);
	 sem_destroy(&full);
	 sem_destroy(&empty);*/

	 comparefunc();


	 return 0;
 }









// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
