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

#include <sstream>
#include "token.hpp"

/*
 * Token constructor
 */
token::token(void) {
	type = UNDEFINED;
	parent = NULL;
}

/*
 * Token constructor
 */
token::token(const token &other) : type(other.type), parent(other.parent) {

	// set attributes
	text.assign(other.text);
	children.assign(other.children.begin(), other.children.end());
}

/*
 * Token constructor
 */
token::token(const unsigned int type, token *parent) {

	// set attributes
	this->type = type;
	this->parent = parent;
}

/*
 * Token constructor
 */
token::token(const std::string &text, const unsigned int type, token *parent) {

	// set attributes
	this->type = type;
	this->parent = parent;
	this->text.assign(text);
}

/*
 * Token constructor
 */
token::token(const std::string &text, const unsigned int type, token *parent, const std::vector<token *> &children) {

	// set attributes
	this->type = type;
	this->parent = parent;
	this->text.assign(text);
	this->children.assign(children.begin(), children.end());
}

/*
 * Token destructor
 */
token::~token(void) {
	text.clear();
	remove_children();
}

/*
 * Token assignment
 */
token &token::operator=(const token &other) {

	// check for self-assignment
	if(this == &other)
		return *this;

	// set attributes
	type = other.type;
	parent = other.parent;
	text.assign(other.text);
	children.assign(other.children.begin(), other.children.end());
	return *this;
}

/*
 * Token equivalence
 */
bool token::operator==(const token &other) {
	unsigned int i;

	// check for same object
	if(this == &other)
		return true;

	// check if all attributes match
	if(type != other.type
			|| text != other.text
			|| parent != other.parent
			|| size() != other.children.size())
		return false;
	for(i = 0; i < size(); i++)
		if(children.at(i) != other.children.at(i))
			return false;
	return true;
}

/*
 * Add child to token children
 */
token *token::add_child(token *child) {

	// return null
	if(!child)
		return NULL;

	// add child to back of children vector & return child
	children.push_back(child);
	return children.back();
}

/*
 * Converts a string to a float
 */
bool token::convert_to_float(mpf_t &out, std::string &str) {

	// check if string is empty
	if(str.empty())
		return false;

	// return conversion
	mpf_init_set_str(out, str.c_str(), 10);
	return true;
}

/*
 * Converts a string to an integer
 */
bool token::convert_to_integer(mpz_t &out, std::string &str) {

	// check if string is empty
	if(str.empty())
		return false;

	// return conversion
	mpz_init_set_str(out, str.c_str(), 10);
	return true;
}

/*
 * Returns a token child at a given index
 */
token *token::get_child(unsigned int index) {

	// check if out-of-bounds
	if(index >= size())
		return NULL;

	// return child at given index
	return children.at(index);
}

/*
 * Negates the current word token's value if possible
 */
bool token::negate(void) {

	// negate the value if of type integer or float
	if(type == INTEGER) {
		mpz_t value;
		convert_to_integer(value, text);
		mpz_neg(value, value);
		convert_to_string(value, text);
		mpz_clear(value);
	} else if(type == FLOAT) {
		mpf_t value;
		convert_to_float(value, text);
		mpf_neg(value, value);
		convert_to_string(value, text);
		mpf_clear(value);
	} else
		return false;
	return true;
}

/*
 * Removes a child held by token at a given index
 */
bool token::remove_child(unsigned int index) {

	// check if out-of-bounds
	if(index >= size())
		return false;
	children.erase(children.begin() + index);
	return true;
}

/*
 * Remove all token children
 */
void token::remove_children(void) {

	// remove children until none exist
	while(!children.empty())
		remove_child(0);
}

/*
 * Returns a string representation of the current state of the token
 */
void token::to_string(std::string &str) {
	std::stringstream ss;

	// convert token type to string
	type_to_string(type, str);

	// append text & size if appropriate
	if(!text.empty()) {
		str.append(": ");
		str.append(text);
	}
	ss << " (" << children.size() << ")";

	// assign to str
	str.append(ss.str());
}

/*
 * Returns a string representation of the given type
 */
void token::type_to_string(unsigned int type, std::string &str) {
	std::stringstream ss;

	// append appropriate type string
	str.append("[");
	switch(type) {
		case UNDEFINED: str.append("UNDEFINED");
			break;
		case ASSIGNMENT: str.append("ASSIGNMENT");
			break;
		case BEGIN: str.append("BEGIN");
			break;
		case BINARY_OPER: str.append("BINARY OPERATOR");
			break;
		case CLOSE_PAREN: str.append("CLOSE PARENTHESIS");
			break;
		case CONSTANT: str.append("CONSTANT");
			break;
		case END: str.append("END");
			break;
		case EXPRESSION: str.append("EXPRESSION");
			break;
		case FLOAT: str.append("FLOAT");
			break;
		case FUNCTION: str.append("FUNCTION");
			break;
		case INTEGER: str.append("INTEGER");
			break;
		case LOGICAL_OPER: str.append("LOGICAL OPERATOR");
			break;
		case OPER: str.append("OPERATOR");
			break;
		case OPEN_PAREN: str.append("OPEN PARENTHESIS");
			break;
		case STRING: str.append("STRING");
			break;
		case UNARY_OPER: str.append("UNARY OPERATOR");
			break;
		default:

			// if the type is unknown, assign the value of the type
			ss << "UNKNOWN (" << type << ")";
			str.append(ss.str());
			break;
	}
	str.append("]");
}
