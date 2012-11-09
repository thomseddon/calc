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
#include <math.h>
#include "calc.h"

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
	int c, i, j, broken = 0;
	double replace = 0.0L;

	/* Skip preceeding whitespace */
	do {
		c = (unsigned char) *s++;
	} while (isspace(c));

	for (i = 0; i < MAX_CONST_LEN; i++, c = (unsigned char) *s++) {
		ct = constants;
		for (j = 0; j < constantCount ; j++, *ct++) {
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


double operation(char operator, double pre, double post)
{
	switch (operator) {
		case '\'': return pow(pre, post);
		case '/': return pre / post;
		case 'x': return pre * post;
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
	double number, total = 0.0L;

	/* TODO: Grow this properly */
	struct Token equation[200];

	/* Tokenise */
	for (i = 1; i <  argc; i++) {
		str = argv[i];

		while (*str != '\0') {
			if (isspace(*str)) {
				/* Space */
				*str++;
			} else if ((number = strtold(str, &str)) == 0.0L && (number = strtoconst(str, &str)) == 0.0L) {
				/* NaN */
				strcpy(equation[equationCounter].type, OP);
				equation[equationCounter].charVal = *str;
				equationCounter++;

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
	findOperations('\'', equation, &equationCounter); //bOdmas
	findOperations('/', equation, &equationCounter); //boDmas
	findOperations('x', equation, &equationCounter); //bodMas
	findOperations('+', equation, &equationCounter); //bodmAs
	findOperations('-', equation, &equationCounter); //bodmaS

	/*
	 * TODO: Increase precision
	 * TODO: Allow precision to be specified
	 */
	printf("\n= %.15g\n", equation[0].intVal);
}