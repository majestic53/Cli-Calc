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

#ifndef TOKEN_HPP_
#define TOKEN_HPP_

#include <string>
#include <vector>

class token {
private:

	unsigned int type;
	std::string text;
	token *parent;
	std::vector<token *> children;

	/*
	 * Returns a string representation of the given type
	 */
	static void type_to_string(unsigned int type, std::string &str);

public:

	/*
	 * Tag types
	 */
	enum TYPE { UNDEFINED, ASSIGNMENT, BEGIN, BINARY_OPER, CLOSE_PAREN, CONSTANT, END, EXPRESSION, FLOAT, FUNCTION, INTEGER,
				LOGICAL_OPER, OPER, OPEN_PAREN, STRING, UNARY_OPER };

	/*
	 * Token constructor
	 */
	token(void);

	/*
	 * Token constructor
	 */
	token(const token &other);

	/*
	 * Token constructor
	 */
	token(const unsigned int type, token *parent);

	/*
	 * Token constructor
	 */
	token(const std::string &text, const unsigned int type, token *parent);

	/*
	 * Token constructor
	 */
	token(const std::string &text, const unsigned int type, token *parent, const std::vector<token *> &children);

	/*
	 * Token destructor
	 */
	virtual ~token(void);

	/*
	 * Token assignment
	 */
	token &operator=(const token &other);

	/*
	 * Token equivalence
	 */
	bool operator==(const token &other);

	/*
	 * Token equivalence
	 */
	bool operator!=(const token &other) { return !(*this == other); }

	/*
	 * Add child to token children
	 */
	token *add_child(token *child);

	/*
	 * Converts a string to another type
	 */
	template<class T>
	static T convert_to(std::string &str);

	/*
	 * Converts a type to a string
	 */
	template<class T>
	static void convert_to_string(T val, std::string &out);

	/*
	 * Returns a token child at a given index
	 */
	token *get_child(unsigned int index);

	/*
	 * Returns all children currently held by the token
	 */
	std::vector<token *> &get_children(void) { return children; }

	/*
	 * Returns the token's parent
	 */
	token *get_parent(void) { return parent; }

	/*
	 * Returns the token's type
	 */
	unsigned int get_type(void) { return type; }

	/*
	 * Returns the token's text
	 */
	std::string &get_text(void) { return text; }

	/*
	 * Negates the current word token's value if possible
	 */
	bool negate(void);

	/*
	 * Removes a child held by token at a given index
	 */
	bool remove_child(unsigned int index);

	/*
	 * Remove all token children
	 */
	void remove_children(void);

	/*
	 * Set the tokens children
	 */
	void set_children(const std::vector<token *> &children) { this->children.assign(children.begin(), children.end()); }

	/*
	 * Set the tokens parent
	 */
	void set_parent(token *parent) { this->parent = parent; }

	/*
	 * Set the tokens text
	 */
	void set_text(const std::string &text) { this->text.assign(text); }

	/*
	 * Set the tokens type
	 */
	void set_type(unsigned int type) { this->type = type; }

	/*
	 * Return the number of children currently held by the token
	 */
	unsigned int size(void) { return children.size(); }

	/*
	 * Returns a string representation of the current state of the token
	 */
	void to_string(std::string &str);
};

#endif
