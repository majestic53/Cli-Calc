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

#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <string>
#include "exc_code.hpp"
#include "lexer.hpp"
#include "syn_tree.hpp"
#include "token.hpp"

class parser {
private:

	lexer lex;
	std::vector<syn_tree *> tree;
	std::string input;

	/*
	 * Add a symbol to the current tree
	 */
	void add_symbol(syn_tree &tree);

	/*
	 * Parse an expression and add it to the current token
	 */
	void e0p(syn_tree &tree);

	/*
	 * Parse an expression and add it to the current token
	 */
	void e1(syn_tree &tree);

	/*
	 * Parse an expression and add it to the current token
	 */
	void e1p(syn_tree &tree);

	/*
	 * Parse an expression and add it to the current token
	 */
	void e2(syn_tree &tree);

	/*
	 * Parse an expression and add it to the current token
	 */
	void e2p(syn_tree &tree);

	/*
	 * Parse an expression and add it to the current token
	 */
	void e3(syn_tree &tree);

	/*
	 * Parse an expression and add it to the current token
	 */
	void e3p(syn_tree &tree);

	/*
	 * Parse an expression and add it to the current token
	 */
	void e4(syn_tree &tree);

	/*
	 * Parse an expression and add it to the current token
	 */
	void e4p(syn_tree &tree);

	/*
	 * Parse an expression and add it to the current token
	 */
	void e5(syn_tree &tree);

	/*
	 * Parse an expression and add it to the current token
	 */
	void e5p(syn_tree &tree);

	/*
	 * Parse an expression and add it to the current token
	 */
	void e6(syn_tree &tree);

	/*
	 * Parse an expression and add it to the current token
	 */
	void e6p(syn_tree &tree);

	/*
	 * Parse an expression and add it to the current token
	 */
	void e7(syn_tree &tree);

	/*
	 * Parse an expression and add it to the current token
	 */
	void expression(syn_tree &tree);

	/*
	 * Parse an identifier and add it to the current token
	 */
	void identifier(syn_tree &tree);

	/*
	 * Parse a statement and add it to the current token
	 */
	void statement(syn_tree &tree);

public:

	/*
	 * Parser constructor
	 */
	parser(void) { return; }

	/*
	 * Parser constructor
	 */
	parser(const parser &other);

	/*
	 * Parser constructor
	 */
	parser(const std::string &input) { this->input.assign(input); }

	/*
	 * Parser destructor
	 */
	virtual ~parser(void) { cleanup(); }

	/*
	 * Parser assignment
	 */
	parser &operator=(const parser &other);

	/*
	 * Parser equivalence
	 */
	bool operator==(const parser &other);

	/*
	 * Parser equivalence
	 */
	bool operator!=(const parser &other) { return !(*this == other); }

	/*
	 * Cleanup after parsing
	 */
	void cleanup(void);

	/*
	 * Returns the buffer's current position in the stream
	 */
	unsigned int get_position(void) { return lex.get_position(); }

	/*
	 * Returns the number of aggrigated commands in the abstract syntax tree
	 */
	unsigned int get_size(void) { return tree.size(); }

	/*
	 * Returns the abstract syntax tree
	 */
	std::vector<syn_tree *> &get_syntax_tree(void) { return tree; }

	/*
	 * Parse input string and/or file
	 */
	void parse(void);

	/*
	 * Resets the parser to initial state
	 */
	void reset(void) { lex.reset(); }

	/*
	 * Returns a string representation of the current state of the parser
	 */
	void to_string(std::string &str);
};

#endif
