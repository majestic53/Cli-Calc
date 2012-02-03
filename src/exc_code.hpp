/*
 * Cli Calc -- a small CLI calculator
 * Copyright (C) 2012 David Jolly
 * ----------------------
 * This file is part of Cli Calc
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EXC_CODE_HPP_
#define EXC_CODE_HPP_

#include <string>

class exc_code {
public:

	/*
	 * General exceptions
	 */
	static const int SUCCESS = 0;
	static const int MEM_FAILURE = 1;
	static const int EXIT = 2;
	static const int STDIN_EOF = 3;

	/*
	 * Parser exceptions
	 */
	static const int EXPECTING_IDENTIFIER = 4;
	static const int EXPECTING_CLOSE_PAREN = 5;
	static const int EXPECTING_CLOSE_BRACKET = 6;

	/*
	 * Runtime exceptions
	 */
	static const int INVALID_OPERATOR = 7;
	static const int INVALID_OPERAND = 8;
	static const int EXPECTING_INTEGER_OPERAND = 9;
	static const int INVALID_BINARY_OPERATOR = 10;
	static const int INVALID_LOGICAL_OPERATOR = 11;
	static const int INVALID_ARITHMETIC_OPERATOR = 12;
	static const int INVALID_EXPRESSION = 13;
	static const int INVALID_CONSTANT = 14;
	static const int INVALID_FUNCTION = 15;
	static const int INVALID_STATEMENT = 16;
	static const int INVALID_ASSIGNMENT_STATEMENT = 17;
	static const int EXPECTING_STRING_IDENTIFIER = 18;
	static const int UNDEFINED_IDENTIFIER = 19;
	static const int EXPECTING_POSITIVE_INTEGER_OPERAND = 20;
	static const int INVALID_UNARY_OPERATOR = 21;

	/*
	 * Exception message
	 */
	static const std::string MESSAGE[];
};

#endif
