// Copyright (c) 2012 Thom Seddon <thom@seddonmedia.co.uk>
//
// This file is part of calc.
//
// calc is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// calc is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with calc.  If not, see <http://www.gnu.org/licenses/>.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calc.h"


long int operation(char operator, long int pre, long int post)
{
	switch (operator){
		case '/': return pre / post;
		case '*': return pre * post;
		case '+': return pre + post;
		case '-': return pre - post;
	}

	printf("Unsupported character");
	exit(1);
}


void findOperations(char operator, struct Token equation[], int *equationCounter)
{
	int i = 0, j;
	while (i < *equationCounter) {
		if (strcmp(equation[i].type, OP) == 0 && equation[i].charVal == operator) {
			if (i == 0 || i == *equationCounter - 1 || strcmp(equation[i - 1].type, NUM)
				|| strcmp(equation[i + 1].type, NUM)) {
					printf("Sytax error.\n");
					exit(1);
			}

			equation[i - 1].intVal = operation(operator, equation[i - 1].intVal, equation[i + 1].intVal);

			for (j = i; j < (*equationCounter - i + 1); j++) {
				/*
				 * Starting from the token after the operator, shift everything up two places:
				 *  - 1 place for the operator
				 *  - 1 place for the post number
				 */
				equation[j] = equation[j + 2];
			}

			*equationCounter = *equationCounter - 2;

		} else {
			i++;
		}
	}

}


void main(int argc, char *argv[])
{
	char *str;
	int i, equationCounter = 0;
	long int number, total = 0;

	/* TODO: Grow this properly */
	struct Token equation[200];

	/* Tokenise */
	for (i = 1; i <  argc; i++) {
		str = argv[i];

		while (*str != '\0') {
			if ((number = strtol(str, &str, 10)) == 0L) {
				/* NaN */

				/* Using '*' seems to screw up argv */
				if (*str == 'x') {
					*str = '*';
				}

				if (*str != ' ') {
					strcpy(equation[equationCounter].type, OP);
					equation[equationCounter].charVal = *str;
					equationCounter++;
				}

				*str++;
			} else {
				/* Number */
				strcpy(equation[equationCounter].type, NUM);
				equation[equationCounter].intVal = number;
				equationCounter++;
			}
		}
	}

	/* Evaluate */
	findOperations('/', equation, &equationCounter); //boDmas
	findOperations('*', equation, &equationCounter); //bodMas
	findOperations('+', equation, &equationCounter); //bodmAs
	findOperations('-', equation, &equationCounter); //bodmaS

	printf("\n= %d\n", equation[0].intVal);
}