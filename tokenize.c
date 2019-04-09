/**
 * Program: tokenize.c
 * Uses a scanner implemented using a transition matrix to perform
 * lexigraphical analysis
 *
 * @author Brennan Reed
 */



#include <stdio.h>
#include "classes.h"
#include <string.h>
#include <stdlib.h>


/**
 * Structure: Transition
 * ---------------------
 * Structure which represents a single transition function
 * to be stored in the transition matrix
 *
 * class: the character class of the transition
 * end: the end location of the transition
 * action: the action associated with the transition
 */

typedef struct{
	int class, end;
	char action;
} Transition;

/**
 * Function: printTransitions
 * ---------------------------
 *  Prints out a formatted representation of the tranition matrix
 *  passd to it
 *
 *  states: the number of states contained in the transition matrix
 *  transitions: the transition matrix to be printed out
 *
 *  returns: void
 */

void printTransitions(int states, Transition transitions[][12]){
	printf("Scanning using the following matrix:\n");
	printf(" ");
	for (int i = 0; i < 12; i++)
		printf("%5d", i);

	for (int i = 0; i < states; i++){
		printf("\n%2d", i);
		for (int x = 0; x < 12; x++)
			printf("%4d%c", transitions[i][x].end, transitions[i][x].action);
	}
	printf("\n");
	return;
}

/**
 * Function: classification
 * ------------------------
 *  Evaluates a specific character and returns the integer associated with
 *  the class of the character
 *
 *  input: character to be evaluated
 *
 *  returns: integer representation of the characters class
 */

int classification(char input){
	int c = (int)input;
	if (c == 32 || c == 9)
		return 0;
	else if (c == 10)
		return 1;
	else if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122) || c == 95)
		return 2;	
	else if (c == 48)
		return 3;
	else if (c >= 49 && c <= 55)
		return 4;
	else if (c == 56 || c == 57)
		return 5;
	else if (c == 47)
		return 6;
	else if (c == 42)
		return 7;
	else if (c == 43 || c == 45 || c == 37)
		return 8;
	else if (c > 0 && c <= 127)
		return 9;
	else if (input == EOF)
		return 10;
	else if (c < 0 || c > 127)
		return 11;
	else 
		return 12;
}

/**
 * Function: matrixBuilder
 * ------------------------
 *  Handles the processing of the transition matrix description file
 *
 *  states: the number of states in the transition matrix
 *  fp: pointer to the transition matrix description file
 *  matrix: the 2-D array to store the transition functions in
 *
 *  returns: void
 */

void matrixBuilder(int states, FILE *fp, Transition matrix[][12]){
	char *ptr, buf[256];
	char slash = '\0', action = '\0';
	int row = 0, col = 0, target = 0;

	for (int i = 0; i < states; i++){
		for (int x = 0; x < 12; x++)
			matrix[i][x].end = -1;
	}

	while((ptr = fgets(buf, 256, fp)) != NULL){
		if (strlen(buf) == 1)
			continue;
		char * token = strtok(ptr, " ");
		row = atoi(token);
		token = strtok(NULL, " ");
		while (token != NULL){
			sscanf(token, "%d%c%d%c", &col, &slash, &target, &action);
			matrix[row][col].class = col;
			matrix[row][col].end = target;
			matrix[row][col].action = action;
			token = strtok(NULL, " ");
		}
	}
	for (int i = 0; i < states; i++){
		for (int x = 0; x < 12; x++){
			if (matrix[i][x].end == -1){
				matrix[i][x].class = x;
				matrix[i][x].end = 99;
				matrix[i][x].action = 'd';
			}
		}
	}
	return;
}

/**
 * Function: scanner
 * -----------------
 *  Performs the necessary steps to read and recognize one token
 *
 *  initial_state: the initial state of the scanner
 *  accept_state: the accept state of the scanner
 *  matrix: 2-D array containing all the transition functions
 *
 *  returns: 1 if EOF, 0 otherwise 
 */

int scanner(int initial_state, int accept_state, Transition matrix[][12]){
	char c = ' ';
	int current = initial_state;
	int accept = accept_state;
	int class, next_state, count = 0, recognize = 0;
	char action, buffer[100];
	printf("%d ", current);	
	while (current != accept){
		c = getchar();
		class = classification(c);
		action = matrix[current][class].action;
		next_state = matrix[current][class].end;
		current = next_state;
		printf("%d ", current);
		if (c == EOF && count == 0){
			printf("EOF\n");
			break;
		}
		if (current == 99){
			c = getchar();
			class = classification(c);
			while (class != 10 && class != 0 && class != 1){
				c = getchar();
				class = classification(c);
			}
			break;
		}
		if (action == 's'){
			buffer[count] = c;
			count++;
		} else {
			if (current == accept){
				buffer[count] = '\0';
				recognize = 1;
			}
		}
	}
	if (recognize == 1)
		printf("recognized '%s'\n", buffer);
	else if (c == EOF)
		return 1;
	else {
		printf("rejected\n");
	}
	return 0;
}

/**
 * Function: main
 * --------------
 *  Gets and passes the input file to the matrix builder, then 
 *  repeatedly calls the scanner to obtain tokens
 *
 *  argc: the number of commandline arguments
 *  argv: array of commandline arguments
 *
 *  returns: 0 if successful
 */

int main(int argc, char*  argv[]){

	FILE *fp;
	char string[10];
	int start, accept, states, stop = 0;
	
	if (argc > 1)
		fp = fopen(argv[1], "r");
	else {
		fprintf(stderr, "%s", "usage: ./tokenize tmfile\n");
		exit(EXIT_FAILURE);
	}
	if (fp == NULL){
		// something went wrong
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}
	fscanf(fp, "%s %d", string, &states);
	fscanf(fp, "%s %d", string, &start);
	fscanf(fp, "%s %d", string, &accept);

	Transition transitionMatrix[states][12];
	matrixBuilder(states, fp, transitionMatrix);
	printTransitions(states, transitionMatrix);
	
	while (stop == 0){
		stop = scanner(start, accept, transitionMatrix);
	}
	free(fp);
	return 0;
}
