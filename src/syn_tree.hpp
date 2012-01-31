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

#ifndef SYN_TREE_HPP_
#define SYN_TREE_HPP_

#include <stack>
#include <string>
#include "token.hpp"

class syn_tree {
private:

	token *root, *cur;
	std::stack<token *> cache;

	/*
	 * Release resources used by tree
	 */
	static bool cleanup_helper(token **root);

	/*
	 * Copy a tree from a given root token
	 */
	static bool copy_helper(syn_tree &copy, token *root, int advance);

	/*
	 * Returns the current pointer
	 */
	token *get_current(void) { return cur; }

	/*
	 * Returns the root pointer
	 */
	token *get_root(void) { return root; }

	/*
	 * Print tree helper function
	 */
	static void print_tree_helper(token *root, std::string &str);

	/*
	 * Returns the current pointer
	 */
	void set_current(token *cur) { this->cur = cur; }

	/*
	 * Returns the root pointer
	 */
	void set_root(token *root) { this->root = root; }

	/*
	 * Compare two trees for equivalence
	 */
	static bool tree_equals(token *root_a, token *root_b);

public:

	/*
	 * Syntax tree constructor
	 */
	syn_tree(void);

	/*
	 * Syntax tree constructor
	 */
	syn_tree(const syn_tree &other);

	/*
	 * Syntax tree constructor
	 */
	syn_tree(token *root);

	/*
	 * Syntax tree constructor
	 */
	syn_tree(token *root, token *cur);

	/*
	 * Syntax tree destructor
	 */
	~syn_tree(void) { cleanup(); }

	/*
	 * Syntax tree assignment
	 */
	syn_tree &operator=(const syn_tree &other);

	/*
	 * Syntax tree equals
	 */
	bool operator==(const syn_tree &other);

	/*
	 * Syntax tree not-equals
	 */
	bool operator!=(const syn_tree &other) { return !(*this == other); }

	/*
	 * Add a child token to the current token
	 */
	bool add_child(token &tok) { return add_child(tok.get_text(), tok.get_type()); }

	/*
	 * Add a child token to the current token
	 */
	bool add_child(const std::string &text, unsigned int type);

	/*
	 * Advance current token backwards (to parent)
	 */
	bool advance_back(void);

	/*
	 * Advance current token forwards (to specified child)
	 */
	bool advance_forward(unsigned int index);

	/*
	 * Advance current token to root
	 */
	bool advance_root(void);

	/*
	 * Returns true if current token is root token
	 */
	bool at_root(void) { return cur == root; }

	/*
	 * Free all used resources in the tree
	 */
	bool cleanup(void);

	/*
	 * Make a copy of current tree
	 */
	bool copy(syn_tree &copy);

	/*
	 * Returns contents at current token
	 */
	bool get_contents(token &tok);

	/*
	 * Returns contents of the specified child token of current token
	 */
	bool get_child_contents(token &tok, unsigned int index);

	/*
	 * Returns the text of the specified child token of current token
	 */
	bool get_child_text(std::string &text, unsigned int index);

	/*
	 * Returns the type of the specified child token of current token
	 */
	bool get_child_type(unsigned int &type, unsigned int index);

	/*
	 * Returns the text of the current token
	 */
	bool get_text(std::string &text);

	/*
	 * Returns the type of the current token
	 */
	bool get_type(unsigned int &type);

	/*
	 * Returns the number of children tokens at current token
	 */
	unsigned int get_size(void) { return cur->size(); }

	/*
	 * Returns the contents at the root
	 */
	bool get_root_contents(token &tok);

	/*
	 * Negate the current token (if possible)
	 */
	bool negate(void);

	/*
	 * Negate child token (if possible)
	 */
	bool negate_child(unsigned int index);

	/*
	 * Set current to old cached token
	 */
	bool pop_cache(void);

	/*
	 * Print a tree from a given root
	 */
	static void print_tree(syn_tree &tree, std::string &str) { print_tree_helper(tree.get_root(), str); }

	/*
	 * Cache old token
	 */
	bool push_cache(void);

	/*
	 * Remove the current token (moves current token to parent token)
	 */
	bool remove(void);

	/*
	 * Removes a child node (recursively) from the current token
	 */
	bool remove_child(unsigned int index);

	/*
	 * Remove all children (recursively) from the current token
	 */
	bool remove_children(void);

	/*
	 * Set the contents of the current token
	 */
	bool set_contents(token &tok);

	/*
	 * Sets the contents of the specified child token of current token
	 */
	bool set_child_contents(token &tok, unsigned int index);

	/*
	 * Sets the text of the specified child token of current token
	 */
	bool set_child_text(std::string &text, unsigned int index);

	/*
	 * Sets the type of the specified child token of current token
	 */
	bool set_child_type(unsigned int type, unsigned int index);

	/*
	 * Set the text of the current token
	 */
	bool set_text(std::string &text);

	/*
	 * Set the type of the current token
	 */
	bool set_type(unsigned int type);

	/*
	 * Returns a string representation of the current state of the token
	 */
	void to_string(std::string &str);
};

#endif /* SYN_TREE_HPP_ */
