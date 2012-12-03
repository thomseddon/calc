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

/* strtoconst */
#define MAX_CONST_LEN 3

struct Constant {
	char name[MAX_CONST_LEN];
	double value;
	int match;
	int matched;
	int caseSensative;
};

/* main */
#define OP "OP"
#define NUM "NUM"

struct Token {
	char type[3];
	char charVal;
	double intVal;
};

struct Equation {
	int size;
	int used;
	struct Token *tokens;
};