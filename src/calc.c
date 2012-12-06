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
#include <ctype.h>
#include <math.h>
#include "calc.h"

void insertToken(struct Equation *equation, char *type, char charVal, double intVal)
{
	if (equation->last == NULL) {
		equation->first = (struct Token *)malloc(sizeof(struct Token));
		equation->last = equation->first;
	} else {
		equation->last->next = (struct Token *)malloc(sizeof(struct Token));
		equation->last->next->previous = equation->last;
		equation->last = equation->last->next;
	}

	equation->last->next = NULL;

	strcpy(equation->last->type, type);
	equation->last->charVal = charVal;
	equation->last->intVal = intVal;
}

/*
 * Parses a C string to return numeric constant value if constant is found
 * Inspiration taken from library functions strto*, namely:
 * - OpenBSD: ftp://ftp.irisa.fr/pub/OpenBSD/src/sys/lib/libsa/strtol.c
 * - Apple: http://www.opensource.apple.com/source/xnu/xnu-1456.1.26/bsd/libkern/strtol.c
 *
 */
double strtoconst(char *nptr, char **endptr)
{
	/*
	 * Constants to check for
	 */
	int constantCount = 3;
	struct Constant constants[] = {
		{"e", 2.71828182845904523536, 1, 0, 1},
		{"pi", 3.14159265358979323846, 2, 0, 0},
		{"phi", (1.0 + pow(5.0, 0.5)) / 2.0, 3, 0, 0} /* Golden Ratio */
	};
	struct Constant *ct;

	char *s = nptr;
	int c, i, j;
	double replace = 0.0L;

	/* Skip preceeding whitespace */
	do {
		c = (unsigned char) *s++;
	} while (isspace(c));

	for (i = 0; i < MAX_CONST_LEN; i++, c = (unsigned char) *s++) {
		ct = constants;
		for (j = 0; j < constantCount ; j++, ct++) {
			if (ct->name[i] != 0 && (ct->caseSensative ? c : tolower(c)) == ct->name[i]) {
				if (ct->match == ++ct->matched) {
					replace = ct->value;
					break;
				}
			}
		}

		if (replace) {
			/* Match has been found */
			break;
		}
	}

	if (endptr != 0) {
		*endptr = (char *) (replace ? s : nptr);
	}

	return replace;
}

char normalise(char operator)
{
	switch (operator) {
		case 'x':
		case '*':
			return '*';
		case '\'':
		case '^':
			return '^';
		default:
			return operator;
	}
}

double operation(char operator, double pre, double post)
{
	switch (operator) {
		case '^': return pow(pre, post);
		case '/': return pre / post;
		case '*': return pre * post;
		case '+': return pre + post;
		case '-': return pre - post;
	}

	printf("Unsupported character");
	exit(1);
}


void findOperations(char *operators, int numOperators, struct Equation *equation)
{
	int i, j;
	char *operator;
	struct Token *token = equation->first, *newNext;

	for(i = 0; token != NULL && token->next != NULL; i++, token = token->next) {

		for (j = 0, operator = operators; j < numOperators; j++, operator = &operators[j]) {
			if (strcmp(token->type, OP) == 0 && token->charVal == *operator) {
				if (token->previous == NULL || token->next == NULL || strcmp(token->next->type, NUM)
					|| strcmp(token->previous->type, NUM)) {
						printf("Sytax error.\n");
						exit(1);
				}

				/*
				 * Rejig linked list
				 *
				 * NUM[1] OP[2] NUM[3]
				 * 1 takes the value of NUM OP NUM
				 * 2 and 3 are bypassed as links are removed
				 */
				token->previous->intVal = operation(token->charVal, token->previous->intVal, token->next->intVal);

				newNext = token->next->next;
				token = token->previous;

				free(token->next->next);
				free(token->next);

				token->next = newNext;
				if (token->next != NULL) {
					token->next->previous = token;
				}

				break;
			}
		}
	}
}


int main(int argc, char *argv[])
{
	char *str;
	int i;
	double number;

	/* Kick off equation array  */
	struct Equation equation;
	equation.last = NULL;

	/* Tokenise */
	for (i = 1; i <  argc; i++) {
		str = argv[i];

		while (*str != '\0') {
			if (isspace(*str)) {
				/* Space */
				str++;
			} else if ((number = strtold(str, &str)) == 0.0L && (number = strtoconst(str, &str)) == 0.0L) {
				/* NaN */
				insertToken(&equation, OP, normalise(*str), 0);
				str++;
			} else {
				/* Number */
				insertToken(&equation, NUM, '\0', number);
			}
		}
	}

	/* Evaluate */
	findOperations("^", 1, &equation); //bOdmas
	findOperations("*/", 2, &equation); //boDMas
	findOperations("+-", 2, &equation); //bodmAS

	/*
	 * TODO: Increase precision
	 * TODO: Allow precision to be specified
	 */
	printf("\n= %.15g\n", equation.first->intVal);

	/* Cleanup */
	free(equation.first);
	equation.first = NULL;

	return 0;
}