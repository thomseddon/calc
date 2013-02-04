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

void stringAlloc(struct String *string, int required)
{
	if (string->count + required >= string->total) {
		string->total += (required > string->blockSize ? required : string->blockSize);
		string->str = realloc(string->str, string->total);
	}
}

void stringAddChar(struct String *string, char ch)
{
	stringAlloc(string, 1);
	string->str[string->count++] = ch;
	string->str[string->count] = '\0';
}

void stringAddStr(struct String *string, char *str)
{
	int len;

	stringAlloc(string, len = strlen(str));

	string->count == 0 ? strcpy(string->str, str) : strcat(string->str, str);
	string->count += len;
}

void addScope(struct Scope *scope, struct Scope **currentScope)
{
	if (scope == NULL) {
		scope = (struct Scope *)malloc(sizeof(struct Scope));
		scope->depth = 0;
		scope->parent = NULL;
		*currentScope = scope;
	} else {
		scope->child = (struct Scope *)malloc(sizeof(struct Scope));
		scope->child->depth = scope->depth + 1;
		scope->child->parent = scope;
		*currentScope = scope->child;
	}

	(*currentScope)->child = NULL;
	(*currentScope)->first = NULL;
	(*currentScope)->last = NULL;
}

void freeScope(struct Scope *scope, struct Scope **currentScope)
{
	struct Token *token = scope->first;

	/* Free tokens */
	if (token != NULL) {
		while(token->next != NULL) {
			token = token->next;
			free(token->previous);
		}
		free(token);
	}

	/*
	 * If there is a parent then we asscend and clean up the child
	 * otherwise we are at the top and can free the current scope
	 */
	*currentScope = scope->parent;
	if (*currentScope != NULL) {
		free((*currentScope)->child);
	} else {
		free(scope);
	}
}

void insertToken(struct Scope *scope, char *type, char charVal, double intVal)
{
	if (scope->last == NULL) {
		scope->first = (struct Token *)malloc(sizeof(struct Token));
		scope->last = scope->first;
	} else {
		scope->last->next = (struct Token *)malloc(sizeof(struct Token));
		scope->last->next->previous = scope->last;
		scope->last = scope->last->next;
	}

	scope->last->next = NULL;

	strcpy(scope->last->type, type);
	scope->last->charVal = charVal;
	scope->last->intVal = intVal;
}

void freeAll(struct Scope *scope)
{
	/* Walk up scopes freeing all tokens */
	while (scope->parent != NULL) {
		freeScope(scope, &scope);
	}

	/* Free root scope */
	freeScope(scope, &scope);
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

	printf("Unknown operator\n");
	exit(1);
}

void findOperations(char *operators, int numOperators, struct Scope *scope)
{
	int i;
	char *operator;
	struct Token *token = scope->first, *newNext;

	for(; token != NULL && token->next != NULL; token = token->next) {
		for (i = 0, operator = operators; i < numOperators; i++, operator = &operators[i]) {
			if (strcmp(token->type, OP) == 0 && token->charVal == *operator) {
				if (token->previous == NULL || token->next == NULL || strcmp(token->next->type, NUM)
					|| strcmp(token->previous->type, NUM)) {
						freeAll(scope);
						printf("Sytax error.\n");
						exit(1);
				}

				/*
				 * Rejig linked list
				 *
				 * NUM[1] OP[2] NUM[3]
				 * 1 takes the value of NUM OP NUM
				 * 2 and 3 are bypassed, freed as links are removed
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

double evaluateScope(struct Scope *scope, struct Scope **currentScope)
{
	double total;

	/* Evaluate */
	findOperations("^", 1, scope); //bOdmas
	findOperations("*/", 2, scope); //boDMas
	findOperations("+-", 2, scope); //bodmAS

	/* Get total */
	total = scope->first == NULL ? 0 : scope->first->intVal;

	/* Cleanup */
	freeScope(scope, currentScope);

	return total;
}

int main(int argc, char *argv[])
{
	char ch, *str, *strStart;
	int i;
	double number;
	struct String input = {NULL, 0, 0, 5};

	/* Kick off scope  */
	struct Scope *currentScope = NULL;
	addScope(currentScope, &currentScope);

	/* Read input from... */
	if (argc == 1) {
		/* stdin */
		while ((ch = getchar()) != '\n' && ch != EOF) {
			stringAddChar(&input, ch);
		}
	} else {
		/* args */
		for (i = 1; i < argc; i++) {
			stringAddStr(&input, argv[i]);
			stringAddChar(&input, ' ');
		}
	}

	/* Tokenise */
	str = input.str;
	while (*str != '\0') {
		strStart = str;
		if (isspace(*str)) {
			/* Space */
			str++;
		} else if (*str == '(') {
			addScope(currentScope, &currentScope);
			str++;
		} else if (*str == ')') {
			insertToken(currentScope, NUM, '\0', evaluateScope(currentScope, &currentScope));
			str++;
		} else if ((number = strtold(str, &str)) == 0.0L && (number = strtoconst(str, &str)) == 0.0L) {
			/* NaN */
			insertToken(currentScope, OP, normalise(*str), 0);
			str++;
		} else {
			/* Number */
			if ((*strStart == '+' || *strStart == '-')
				&& currentScope->last != NULL
				&& strcmp(currentScope->last->type, OP)) {
				//Assume this was supposed to be an operation
				insertToken(currentScope, OP, *strStart, 0);
			}
			insertToken(currentScope, NUM, '\0', number);
		}
	}

	/*
	 * TODO: Increase precision
	 * TODO: Allow precision to be specified
	 */
	printf("%.15g\n", evaluateScope(currentScope, &currentScope));

	free(input.str);

	return 0;
}