/*
 * inspiredLOGO -- a small LOGO interpreter
 * Copyright (C) 2012 David Jolly
 * ----------------------
 * This file is part of inspiredLOGO
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

#include "parser.hpp"

/*
 * Parser constructor
 */
parser::parser(const parser &other) {
	input.assign(other.input);
	lex = lexer(input);
	tree.clear();
}

/*
 * Parser assignment
 */
parser &parser::operator=(const parser &other) {

	// check for self-assignment
	if(this == &other)
		return *this;

	// set attributes
	input.assign(other.input);
	lex = lexer(input);
	tree.clear();
	return *this;
}

/*
 * Parser equivalence
 */
bool parser::operator==(const parser &other) {
	unsigned int i;

	// check for same object
	if(this == &other)
		return true;

	// check if all attributes match
	if(input != other.input
			|| lex != other.lex
			|| tree.size() != other.tree.size())
		return false;

	// iterate through tree to make sure all entries match
	for(i = 0; i < tree.size(); i++)
		if(tree.at(i) != other.tree.at(i))
			return false;
	return true;
}

/*
 * Add a symbol to the current tree
 */
void parser::add_symbol(syn_tree &tree) {
	token tok;

	// retrieve the current token
	lex.get_token(tok);

	// add it to the tree
	tree.add_child(tok);
	tree.advance_forward(tree.get_size() - 1);
	tree.add_child("", token::EXPRESSION);
	tree.advance_forward(0);

	// advance to the next token
	lex.next();
}

/*
 * Cleanup after parsing
 */
void parser::cleanup(void) {
	input.clear();
	for(std::vector<syn_tree *>::iterator i = tree.begin(); i != tree.end(); ++i) {
		(*i)->cleanup();
		delete (*i);
	}
	tree.clear();
}

/*
 * Parse an expression and add it to the current token
 */
void parser::e0p(syn_tree &tree) {

	// parse for binary operators
	if(lex.get_type() == token::BINARY_OPER) {
		tree.push_cache();
		add_symbol(tree);
		e1(tree);
		e0p(tree);
		tree.pop_cache();
	}
}

/*
 * Parse an expression and add it to the current token
 */
void parser::e1(syn_tree &tree) {
	e2(tree);
	e1p(tree);
}

/*
 * Parse an expression and add it to the current token
 */
void parser::e1p(syn_tree &tree) {

	// parse for logical operators
	if(lex.get_type() == token::LOGICAL_OPER) {
		tree.push_cache();
		add_symbol(tree);
		e2(tree);
		e1p(tree);
		tree.pop_cache();
	}
}

/*
 * Parse an expression and add it to the current token
 */
void parser::e2(syn_tree &tree) {
	e3(tree);
	e2p(tree);
}

/*
 * Parse an expression and add it to the current token
 */
void parser::e2p(syn_tree &tree) {

	// parse for minus operator
	if(lex.get_type() == token::OPER
			&& lex.get_text() == lexer::OPER_DATA[lexer::MINUS]) {
		tree.push_cache();
		add_symbol(tree);
		e3(tree);
		e2p(tree);
		tree.pop_cache();
	}
}

/*
 * Parse an expression and add it to the current token
 */
void parser::e3(syn_tree &tree) {
	e4(tree);
	e3p(tree);
}

/*
 * Parse an expression and add it to the current token
 */
void parser::e3p(syn_tree &tree) {

	// parse for plus operator
	if(lex.get_type() == token::OPER
			&& lex.get_text() == lexer::OPER_DATA[lexer::PLUS]) {
		tree.push_cache();
		add_symbol(tree);
		e4(tree);
		e3p(tree);
		tree.pop_cache();
	}
}

/*
 * Parse an expression and add it to the current token
 */
void parser::e4(syn_tree &tree) {
	e5(tree);
	e4p(tree);
}

/*
 * Parse an expression and add it to the current token
 */
void parser::e4p(syn_tree &tree) {

	// parse for divide/modulo operator
	if(lex.get_type() == token::OPER
			&& (lex.get_text() == lexer::OPER_DATA[lexer::DIV]
			|| lex.get_text() == lexer::OPER_DATA[lexer::MOD])) {
		tree.push_cache();
		add_symbol(tree);
		e5(tree);
		e4p(tree);
		tree.pop_cache();
	}
}

/*
 * Parse an expression and add it to the current token
 */
void parser::e5(syn_tree &tree) {
	e6(tree);
	e5p(tree);
}

/*
 * Parse an expression and add it to the current token
 */
void parser::e5p(syn_tree &tree) {

	// parse for multiply operator
	if(lex.get_type() == token::OPER
			&& lex.get_text() == lexer::OPER_DATA[lexer::MULTI]) {
		tree.push_cache();
		add_symbol(tree);
		e6(tree);
		e5p(tree);
		tree.pop_cache();
	}
}

/*
 * Parse an expression and add it to the current token
 */
void parser::e6(syn_tree &tree) {
	e7(tree);
	e6p(tree);
}

/*
 * Parse an expression and add it to the current token
 */
void parser::e6p(syn_tree &tree) {

	// parse for power operator
	if(lex.get_type() == token::OPER
			&& lex.get_text() == lexer::OPER_DATA[lexer::POW]) {
		tree.push_cache();
		add_symbol(tree);
		e7(tree);
		e6p(tree);
		tree.pop_cache();
	}
}

/*
 * Parse an expression and add it to the current token
 */
void parser::e7(syn_tree &tree) {
	token tok;

	// open parenthesis leads to an expression, followed by a closing paranthesis
	if(lex.get_type() == token::OPEN_PAREN) {
		lex.next();
		expression(tree);
		if(lex.get_type() != token::CLOSE_PAREN)
			throw exc_code::EXPECTING_CLOSE_PAREN;
		lex.next();

	// parse a constant
	} else if(lex.get_type() == token::CONSTANT) {
		lex.get_token(tok);
		tree.add_child(tok);
		lex.next();

	// parse a function or unary operators
	} else if(lex.get_type() == token::FUNCTION
			|| (lex.get_type() == token::UNARY_OPER
			&& lex.get_text() == lexer::UNARY_OPER_DATA[lexer::NOT])) {;
		lex.get_token(tok);
		tree.add_child(tok);
		tree.advance_forward(tree.get_size() - 1);
		lex.next();
		expression(tree);

	// parse an identfier
	} else
		identifier(tree);
}

/*
 * Parse an expression and add it to the current token
 */
void parser::expression(syn_tree &tree) {

	// add a new expression token
	tree.push_cache();
	tree.add_child("", token::EXPRESSION);
	tree.advance_forward(tree.get_size() - 1);
	e1(tree);
	e0p(tree);
	tree.pop_cache();
}

/*
 * Parse an identifier and add it to the current token
 */
void parser::identifier(syn_tree &tree) {
	token tok;

	// add identifier token
	if(lex.get_type() == token::INTEGER
			|| lex.get_type() == token::FLOAT
			|| lex.get_type() == token::STRING) {
		lex.get_token(tok);
		tree.add_child(tok);
	} else
		throw exc_code::EXPECTING_IDENTIFIER;
	lex.next();
}

/*
 * Parse input string and/or file
 */
void parser::parse(void) {

	// initialize lexer
	lex = lexer(input);
	lex.initialize();
	tree.clear();

	// parse all statements and place them in the abstract syntax tree
	while(lex.has_next()) {
		syn_tree *curr = new syn_tree;
		if(!curr)
			throw exc_code::MEM_FAILURE;
		try {
			statement(*curr);
		} catch(int e) {
			delete curr;
			throw;
		}
		curr->advance_root();
		tree.push_back(curr);
	}
}

/*
 * Parse a statement and add it to the current token
 */
void parser::statement(syn_tree &tree) {
	token tok;

	// evaluate as an assignment
	if(lex.get_type() == token::ASSIGNMENT) {
		lex.get_token(tok);
		tree.add_child(tok);
		lex.next();
		if(lex.get_type() != token::STRING)
			throw exc_code::EXPECTING_STRING_IDENTIFIER;
		lex.get_token(tok);
		tree.add_child(tok);
		lex.next();
		expression(tree);

	// evaluate an expression
	} else
		expression(tree);
}

/*
 * Returns a string representation of the current state of the parser
 */
void parser::to_string(std::string &str) {
	std::vector<syn_tree *>::iterator i;

	// form a string representation of the tree
	if(tree.empty())
		str.assign("Empty\n");
	else
		for(i = tree.begin(); i != tree.end(); ++i) {
			syn_tree::print_tree(**i, str);
			str.append("\n");
		}
}
