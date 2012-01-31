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

#include <iostream>
#include <queue>
#include <sstream>
#include "syn_tree.hpp"

/*
 * Syntax tree constructor
 */
syn_tree::syn_tree(void) {

	// assign new attributes
	root = NULL;
	cur = NULL;
}

/*
 * Syntax tree constructor
 */
syn_tree::syn_tree(const syn_tree &other) {

	// assign new attributes
	root = other.root;
	cur = other.cur;
	cache = other.cache;
}

/*
 * Syntax tree constructor
 */
syn_tree::syn_tree(token *root) {

	// assign new attributes
	this->root = root;
	this->cur = root;
}

/*
 * Syntax tree constructor
 */
syn_tree::syn_tree(token *root, token *cur) {

	// assign new attributes
	this->root = root;
	this->cur = cur;
}

/*
 * Syntax tree assignment
 */
syn_tree &syn_tree::operator=(const syn_tree &other) {

	// check for self-assignment
	if(*this == other)
		return *this;

	// cleanup old tree
	cleanup();

	// assign new attributes
	root = other.root;
	cur = other.cur;
	cache = other.cache;
	return *this;
}

/*
 * Syntax tree equals
 */
bool syn_tree::operator==(const syn_tree &other) {

	// check for self
	if(*this == other)
		return true;

	// check that all tokens are the same
	return tree_equals(root, other.root);
}

/*
 * Add a child token to the current token
 */
bool syn_tree::add_child(const std::string &text, unsigned int type) {

	// make sure allocation does not fail
	token *child = new token(text, type, cur);
	if(!child)
		return false;

	// add the child to current token
	if(!cur)
		root = cur = child;
	else
		cur->add_child(child);
	return true;
}

/*
 * Advance current token backwards (to parent)
 */
bool syn_tree::advance_back(void) {

	// check to make sure parent token exists
	if(!cur
			|| cur == root)
		return false;

	// advance current token to parent token
	cur = cur->get_parent();
	return true;
}

/*
 * Advance current token forwards (to specified child)
 */
bool syn_tree::advance_forward(unsigned int index) {

	// check to make sure child token exists
	if(!cur
			|| !get_size()
			|| index >= get_size())
		return false;

	// advance current to child
	cur = cur->get_child(index);
	return true;
}

/*
 * Advance current token to root
 */
bool syn_tree::advance_root(void) {

	// check to make sure current is not already at root
	if(cur == root)
		return false;

	// advance current token to root token
	cur = root;
	return true;
}

/*
 * Free all used resources in the tree
 */
bool syn_tree::cleanup(void) {
	bool res;

	// clear all other attributes
	res = cleanup_helper(&root);
	if(res)
		cur = NULL;
	return res;
}

/*
 * Release resources used by tree
 */
bool syn_tree::cleanup_helper(token **root) {
	unsigned int children;

	// check that root token exists
	if(!(*root))
		return 0;

	children = (*root)->size();

	// remove all children of current token
	for(unsigned int i = 0; i < children; i++) {
		token *child = (*root)->get_child(i);
		if(!cleanup_helper(&child))
			return false;
	}

	// remove child tokens
	(*root)->remove_children();

	// remove root token
	delete (*root);
	(*root) = NULL;
	return true;
}

/*
 * Make a copy of current tree
 */
bool syn_tree::copy(syn_tree &copy) {

	// attempt to copy tree
	bool res = copy_helper(copy, get_root(), -1);

	// return results
	if(res)
		copy.advance_root();
	return res;
}

/*
 * Copy a tree from a given root token
 */
bool syn_tree::copy_helper(syn_tree &copy, token *root, int advance) {

	// check to make sure root token exists
	if(!root)
		return false;

	// add root & all children
	copy.add_child(*root);
	if(advance != -1)
		copy.advance_forward(advance);
	for(unsigned int i = 0; i < root->size(); i++) {
		token *child = root->get_child(i);
		if(!copy_helper(copy, child, i))
			return false;
	}
	if(advance != -1)
		copy.advance_back();
	return true;
}

/*
 * Returns contents at current token
 */
bool syn_tree::get_contents(token &tok) {

	// check that current token exists
	if(!cur)
		return false;

	// set the text & type
	tok.set_text(cur->get_text());
	tok.set_type(cur->get_type());
	return true;
}

/*
 * Returns contents of the specified child token of current token
 */
bool syn_tree::get_child_contents(token &tok, unsigned int index) {

	// check to make sure child token exists
	if(!cur
			|| !get_size()
			|| index >= get_size())
		return false;

	// retrieve contents
	token *child = cur->get_child(index);
	tok.set_text(child->get_text());
	tok.set_type(child->get_type());
	return true;
}

/*
 * Returns the text of the specified child token of current token
 */
bool syn_tree::get_child_text(std::string &text, unsigned int index) {

	// check to make sure child token exists
	if(!cur
			|| !get_size()
			|| index >= get_size())
		return false;

	// retrieve child token text
	token *child = cur->get_child(index);
	text = child->get_text();
	return true;
}

/*
 * Returns the type of the specified child token of current token
 */
bool syn_tree::get_child_type(unsigned int &type, unsigned int index) {

	// check to make sure child token exists
	if(!cur
			|| !get_size()
			|| index >= get_size())
		return false;

	// retrieve child token type
	token *child = cur->get_child(index);
	type = child->get_type();
	return true;
}

/*
 * Returns the text of the current token
 */
bool syn_tree::get_text(std::string &text) {

	// check that root token exists
	if(!root)
		return false;

	// retrieve text
	text = cur->get_text();
	return true;
}

/*
 * Returns the type of the current token
 */
bool syn_tree::get_type(unsigned int &type) {

	// check that root token exists
	if(!root)
		return false;

	// retrieve type
	type = cur->get_type();
	return true;
}

/*
 * Returns the contents at the root
 */
bool syn_tree::get_root_contents(token &tok) {

	// check that root token exists
	if(!root)
		return false;

	// set the text & type
	tok.set_text(root->get_text());
	tok.set_type(root->get_type());
	return true;
}

/*
 * Negate the current token (if possible)
 */
bool syn_tree::negate(void) {

	// check that current token exists
	if(!cur)
		return false;

	// negate current token
	return cur->negate();
}

/*
 * Negate child token (if possible)
 */
bool syn_tree::negate_child(unsigned int index) {

	// check to make sure child token exists
	if(!cur
			|| !get_size()
			|| index >= get_size())
		return false;

	// negate child token
	return cur->get_child(index)->negate();
}

/*
 * Set current to old cached token
 */
bool syn_tree::pop_cache(void) {

	// check that cache is not empty
	if(cache.empty())
		return false;

	// replace current token pointer with cached pointer
	cur = cache.top();
	cache.pop();
	return true;
}

/*
 * Print tree helper function
 */
void syn_tree::print_tree_helper(token *root, std::string &str) {
	std::string tok_str;
	std::queue<token *> que;
	std::vector<token *>::iterator i;

	// check that root token exists
	if(!root)
		return;

	// run through queue breadth-first
	que.push(root);
	while(!que.empty()) {
		token *tok = que.front();
		que.pop();
		tok_str.clear();
		tok->to_string(tok_str);
		str.append(tok_str);
		str.append("\n");
		i = tok->get_children().begin();
		for(; i != tok->get_children().end(); ++i)
			que.push(*i);
	}
}

/*
 * Cache old token
 */
bool syn_tree::push_cache(void) {

	// push current token pointer onto stack
	cache.push(cur);
	return true;
}

/*
 * Remove the current token (moves current token to parent token)
 */
bool syn_tree::remove(void) {
	token *parent = NULL;

	// check that current token exists
	if(!cur)
		return false;

	// remove current token and move to parent token
	if(!cur->get_parent()) {
		cleanup_helper(&cur);
		root = cur;
	} else {
		parent = cur->get_parent();
		for(unsigned int pos = 0; pos < parent->size(); pos++)
			if(parent->get_child(pos) == cur) {
				parent->remove_child(pos);
				break;
			}
		cleanup_helper(&cur);
		cur = parent;
	}
	return true;
}

/*
 * Removes a child node (recursively) from the current token
 */
bool syn_tree::remove_child(unsigned int index) {

	// check that current token exists
	if(!cur)
		return false;

	// check that index is with-in-bounds
	if(!get_size()
			|| index >= get_size())
		return false;


	// cleanup
	token *child = cur->get_child(index);
	cur->remove_child(index);
	cleanup_helper(&child);
	return true;
}

/*
 * Remove all children (recursively) from the current token
 */
bool syn_tree::remove_children(void) {

	// iterate though all children
	while(cur->size())
		if(!remove_child(0))
			return false;
	return true;
}

/*
 * Set the contents of the current token
 */
bool syn_tree::set_contents(token &tok) {

	// make sure current token exists
	if(!cur)
		return false;

	// set current token contents
	cur->set_text(tok.get_text());
	cur->set_type(tok.get_type());
	return true;
}

/*
 * Sets the contents of the specified child token of current token
 */
bool syn_tree::set_child_contents(token &tok, unsigned int index) {

	// check to make sure child token exists
	if(!cur
			|| !get_size()
			|| index >= get_size())
		return false;

	// set contents
	token *child = cur->get_child(index);
	child->set_text(tok.get_text());
	child->set_type(tok.get_type());
	return true;
}

/*
 * Sets the text of the specified child token of current token
 */
bool syn_tree::set_child_text(std::string &text, unsigned int index) {

	// check to make sure child token exists
	if(!cur
			|| !get_size()
			|| index >= get_size())
		return false;

	// set child text
	token *child = cur->get_child(index);
	child->set_text(text);
	return true;
}

/*
 * Sets the type of the specified child token of current token
 */
bool syn_tree::set_child_type(unsigned int type, unsigned int index) {

	// check to make sure child token exists
	if(!cur
			|| !get_size()
			|| index >= get_size())
		return false;

	// set child type
	token *child = cur->get_child(index);
	child->set_type(type);
	return true;
}

/*
 * Set the text of the current token
 */
bool syn_tree::set_text(std::string &text) {

	// make sure current token exists
	if(!cur)
		return false;

	// set text
	cur->set_text(text);
	return true;
}

/*
 * Set the type of the current token
 */
bool syn_tree::set_type(unsigned int type) {

	// make sure current token exists
	if(!cur)
		return false;

	// set type
	cur->set_type(type);
	return true;
}

/*
 * Returns a string representation of the current state of the tree
 */
void syn_tree::to_string(std::string &str) {
	std::stringstream ss;

	// generate string representation
	ss << "R: " << root << std::endl << "C: " << cur << std::endl << "CC (SIZE): " << cache.size() << std::endl;
	str = ss.str();
}

/*
 * Compare two trees for equivalence
 */
bool syn_tree::tree_equals(token *root_a, token *root_b) {
	unsigned int i;
	token *curr_a, *curr_b;
	std::queue<token *> que_a, que_b;

	// check to make sure root A & B exist
	if(!root_a
			|| !root_b)
		return false;

	// run through tokens bread-first
	que_a.push(root_a);
	que_b.push(root_b);
	if(root_a != root_b)
		return false;
	while(!que_a.empty()
			&& !que_b.empty()) {
		curr_a = que_a.front();
		curr_b = que_b.front();
		que_a.pop();
		que_b.pop();
		if(curr_a != curr_b
				|| curr_a->size() != curr_b->size())
			return false;
		for(i = 0; i < curr_a->size(); i++) {
			que_a.push(curr_a->get_child(i));
			que_b.push(curr_b->get_child(i));
		}
	}
	return true;
}
