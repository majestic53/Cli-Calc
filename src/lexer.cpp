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

#include "lexer.hpp"

/*
 * Assignment keyword
 */
const std::string lexer::ASSIGN("make");

/*
 * Binary operator keywords
 */
const std::string lexer::BINARY_OPER_DATA[3] = { "&", "|", "$" };
const std::set<std::string> lexer::BINARY_OPER(lexer::BINARY_OPER_DATA, lexer::BINARY_OPER_DATA + 3);

/*
 * Constant keywords
 */
const std::string lexer::CONSTANT_OPER_DATA[3] = { "e", "pi", "rand" };
const std::set<std::string> lexer::CONSTANT(lexer::CONSTANT_OPER_DATA, lexer::CONSTANT_OPER_DATA + 3);

/*
 * Function keywords
 */

const std::string lexer::FUNCTION_OPER_DATA[22] = { "abs", "acos", "asin", "atan", "ceiling", "cos", "cosh", "fact",
		"fib", "float", "floor", "int", "ln", "log2", "log10", "round", "sin", "sinh", "sqr", "sqrt", "tan", "tanh" };
const std::set<std::string> lexer::FUNCTION(lexer::FUNCTION_OPER_DATA, lexer::FUNCTION_OPER_DATA + 22);

/*
 * Logical operator keywords
 */
const std::string lexer::LOGICAL_OPER_DATA[2] = { "<<", ">>" };
const std::set<std::string> lexer::LOGICAL_OPER(lexer::LOGICAL_OPER_DATA, lexer::LOGICAL_OPER_DATA + 2);

/*
 * Operator keywords
 */
const std::string lexer::OPER_DATA[6] = { "+", "-", "*", "/", "%", "^" };
const std::set<std::string> lexer::OPER(lexer::OPER_DATA, lexer::OPER_DATA + 6);

/*
 * Unary operator keywords
 */
const std::string lexer::UNARY_OPER_DATA[1] = { "~" };
const std::set<std::string> lexer::UNARY_OPER(lexer::UNARY_OPER_DATA, lexer::UNARY_OPER_DATA + 1);

/*
 * Lexer constructor
 */
lexer::lexer(void) {
	type = token::UNDEFINED;
}

/*
 * Lexer constructor
 */
lexer::lexer(const lexer &other) {

	// set attributes
	text.clear();
	type = token::BEGIN;
	input.assign(other.input);
	buff = pb_buffer(input);
}

/*
 * Lexer constructor
 */
lexer::lexer(const std::string &input) {

	// set attributes
	type = token::BEGIN;
	this->input.assign(input);
}

/*
 * Lexer destructor
 */
lexer::~lexer(void) {
	text.clear();
	input.clear();
}

/*
 * Lexer assignment
 */
lexer &lexer::operator=(const lexer &other) {

	// check for self-assignment
	if(this == &other)
		return *this;

	// set attributes
	text.clear();
	type = token::BEGIN;
	input.assign(other.input);
	buff = pb_buffer(input);
	return *this;
}

/*
 * Lexer equivalence
 */
bool lexer::operator==(const lexer &other) {

	// check for same object
	if(this == &other)
		return true;

	// check if all attributes match
	return type == other.type
			&& text == other.text
			&& buff == other.buff
			&& input == other.input;
}

/*
 * Returns the contents of the lexer as a token
 */
bool lexer::get_token(token &tok) {

	// set appropriate values
	tok.set_text(text);
	tok.set_type(type);
	return true;
}

/*
 * Initialize lexer
 */
void lexer::initialize(void) {

	// initialize
	buff = pb_buffer(input);
	buff.initialize();
	text.clear();
	next();
}

/*
 * Advances the lexer one token forward, returning true if one exists
 */
bool lexer::next(void) {

	// remove whitespace from stream
	remove_whitespace();
	text.clear();

	// Parse next character in stream to determine the tokens likely type
	if(!buff.has_next())
		type = token::END;
	else if(isdigit(buff.get_current()))
		number();
	else if(isalpha(buff.get_current()))
		phrase();
	else
		symbol();
	return true;
}

/*
 * Reads a number from the buffer
 */
void lexer::number(void) {
	char ch = buff.get_current();

	// set type
	type = token::INTEGER;

	// aggregate token value
	while(isdigit(ch)) {
		text += ch;
		buff >> ch;
		if(!buff.has_next())
			return;
	}

	// if a decimal point is found, token is of type float
	if(ch == DEC) {
		type = token::FLOAT;
		text += ch;
		buff >> ch;
		while(isdigit(ch)) {
			text += ch;
			buff >> ch;
			if(!buff.has_next())
				return;
		}
	}
}

/*
 * Reads a phrase from the buffer
 */
void lexer::phrase(void) {
	char ch = buff.get_current();

	// set type
	type = token::STRING;

	// aggregate token value
	while(isalnum(ch)) {
		text += ch;
		buff >> ch;
		if(!buff.has_next()) {

			// set appropriate type
			if(is_constant())
				type = token::CONSTANT;
			else if(is_function())
				type = token::FUNCTION;
			else if(text == ASSIGN)
				type = token::ASSIGNMENT;
			return;
		}
	}

	// set appropriate type
	if(is_constant())
		type = token::CONSTANT;
	else if(is_function())
		type = token::FUNCTION;
	else if(text == ASSIGN)
		type = token::ASSIGNMENT;
}

/*
 * Advances the buffer through interveining whitespace between tokens
 */
void lexer::remove_whitespace(void) {
	if(!buff.has_next())
		return;

	// move stream over whitespace characters
	while(isspace(buff.get_current())) {
		if(!buff.has_next())
			return;
		buff.next();
	}
}

/*
 * Resets the lexer to initial state
 */
void lexer::reset(void) {

	// reset the entire lexer depending on the initial input
	type = token::BEGIN;
	text.clear();
	buff.reset();

	// initialize
	text.clear();
	next();
}

/*
 * Reads a symbol from the buffer
 */
void lexer::symbol(void) {
	char ch = buff.get_current();

	// aggregate token value
	text += ch;
	if(is_operator())
		type = token::OPER;
	else
		if(is_binary_operator())
			type = token::BINARY_OPER;
		else if(is_unary_operator())
			type = token::UNARY_OPER;
		else

			// determine the tokens type based off complete token value
			switch(ch) {
				case CLS_PAREN: type = token::CLOSE_PAREN;
					break;
				case OPN_PAREN: type = token::OPEN_PAREN;
					break;
				default: type = token::UNDEFINED;
					buff.next();
					if(!buff.has_next())
						return;
					text += buff.get_current();
					if(is_logical_operator())
						type = token::LOGICAL_OPER;
					else {
						type = token::UNDEFINED;
						text.assign(text.begin(), text.end() - 1);
						return;
					}
					break;
			}

	// advance the buffer to the next character
	buff.next();
}

/*
 * Returns a string representation of the current state of the lexer
 */
void lexer::to_string(std::string &str) {
	token tok;
	get_token(tok);

	// form the string reprentation of the current token
	tok.to_string(str);
}
