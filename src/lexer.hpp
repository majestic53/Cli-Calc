/*
 * Calc -- a small CLI calculator
 * Copyright (C) 2012 David Jolly
 * ----------------------
 * This file is part of Calc
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

#ifndef LEX_HPP_
#define LEX_HPP_

#include <set>
#include <string>
#include "pb_buffer.hpp"
#include "token.hpp"

class lexer {
private:

	unsigned int type;
	std::string text;
	std::string input;
	pb_buffer buff;

	/*
	 * Check to see if an element exists within a set
	 */
	template<class T>
	static bool contains(const std::set<T> &set, const T &element) { return set.find(element) != set.end(); }

	/*
	 * Returns true if the current token is a binary operator
	 */
	bool is_binary_operator(void) { return lexer::contains<std::string>(BINARY_OPER, text); }

	/*
	 * Returns true if the current token is a constant
	 */
	bool is_constant(void) { return lexer::contains<std::string>(CONSTANT, text); }

	/*
	 * Returns true if the current token is a function
	 */
	bool is_function(void) { return lexer::contains<std::string>(FUNCTION, text); }

	/*
	 * Returns true if the current token is a logical operator
	 */
	bool is_logical_operator(void) { return lexer::contains<std::string>(LOGICAL_OPER, text); }

	/*
	 * Returns true if the current token is an operator
	 */
	bool is_operator(void) { return lexer::contains<std::string>(OPER, text); }

	/*
	 * Returns true if the current token is a unary operator
	 */
	bool is_unary_operator(void) { return lexer::contains<std::string>(UNARY_OPER, text); }

	/*
	 * Reads a number from the buffer
	 */
	void number(void);

	/*
	 * Reads a phrase from the buffer
	 */
	void phrase(void);

	/*
	 * Advances the buffer through interveining whitespace between tokens
	 */
	void remove_whitespace(void);

	/*
	 * Reads a symbol from the buffer
	 */
	void symbol(void);

public:

	/*
	 * Assignment keyword
	 */
	static const std::string ASSIGN;

	/*
	 * Binary operator keywords
	 */
	enum BIN_OPER { AND, OR, XOR };
	static const std::string BINARY_OPER_DATA[];
	static const std::set<std::string> BINARY_OPER;

	/*
	 * Close paranthesis character
	 */
	static const char CLS_PAREN = ')';

	/*
	 * Constant keywords
	 */
	enum CONST_OPER { E, PI, RAND };
	static const std::string CONSTANT_OPER_DATA[];
	static const std::set<std::string> CONSTANT;

	/*
	 * Decimal character
	 */
	static const char DEC = '.';

	/*
	 * Function keywords
	 */
	enum FUNCT_OPER { ABS, ACOS, ASIN, ATAN, CEILING, COS, COSH, FACT, FIB, FLOAT, FLOOR, INT, LN, LOG2, LOG10, ROUND,
						SIN, SINH, SQR, SQRT, TAN, TANH };
	static const std::string FUNCTION_OPER_DATA[];
	static const std::set<std::string> FUNCTION;

	/*
	 * Logical operator keywords
	 */
	enum LOG_OPER { LEFT_SHIFT, RIGHT_SHIFT };
	static const std::string LOGICAL_OPER_DATA[];
	static const std::set<std::string> LOGICAL_OPER;

	/*
	 * Open parenthesis character
	 */
	static const char OPN_PAREN = '(';

	/*
	 * Operator keywords
	 */
	enum OPER { PLUS, MINUS, MULTI, DIV, MOD, POW };
	static const std::string OPER_DATA[];
	static const std::set<std::string> OPER;

	/*
	 * Unary operator keywords
	 */
	enum UNARY_OPER { NOT };
	static const std::string UNARY_OPER_DATA[];
	static const std::set<std::string> UNARY_OPER;

	/*
	 * Lexer constructor
	 */
	lexer(void);

	/*
	 * Lexer constructor
	 */
	lexer(const lexer &other);

	/*
	 * Lexer constructor
	 */
	lexer(const std::string &input);

	/*
	 * Lexer destructor
	 */
	virtual ~lexer(void);

	/*
	 * Lexer assignment
	 */
	lexer &operator=(const lexer &other);

	/*
	 * Lexer equivalence
	 */
	bool operator==(const lexer &other);

	/*
	 * Lexer equivalence
	 */
	bool operator!=(const lexer &other) { return !(*this == other); }

	/*
	 * Returns the buffer's current position in the stream
	 */
	unsigned int get_position(void) { return buff.get_position(); }

	/*
	 * Returns the contents of the lexer as a token
	 */
	bool get_token(token &tok);

	/*
	 * Returns the current token text
	 */
	std::string &get_text(void) { return text; }

	/*
	 * Returns the current token type
	 */
	unsigned int get_type(void) { return type; }

	/*
	 * Returns the status of the lexer
	 */
	bool has_next(void) { return type != token::END; }

	/*
	 * Initialize lexer
	 */
	void initialize(void);

	/*
	 * Advances the lexer one token forward, returning true if one exists
	 */
	bool next(void);

	/*
	 * Resets the lexer to initial state
	 */
	void reset(void);

	/*
	 * Returns a string representation of the current state of the lexer
	 */
	void to_string(std::string &str);
};

#endif
