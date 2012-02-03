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

#include <iostream>
#include <sstream>
#include "exc_code.hpp"
#include "sym_table.hpp"

/*
 * Symbol table assignment
 */
sym_table &sym_table::operator=(const sym_table &other) {

	// check for self-assignment
	if(*this == other)
		return *this;

	cleanup();
	table = other.table;
	return *this;
}

/*
 * Symbol table equivalence
 */
bool sym_table::operator==(const sym_table &other) {
	std::map<std::string, token *>::iterator a = table.begin();
	std::map<std::string, token *>::const_iterator b = other.table.begin();

	// check for same object
	if(*this == other)
		return true;

	// check that sizes are the same
	if(table.size() != other.table.size())
		return false;

	// check that contents is the same
	for(; a != table.end(); ++a, ++b)
		if((*a->second) != (*b->second))
			return false;
	return true;
}

/*
 * Cleanup resources used by symbol table
 */
void sym_table::cleanup(void) {
	std::map<std::string, token *>::iterator i = table.begin();

	// release all resourced used by table
	for(; i != table.end(); ++i)
		delete i->second;
	table.clear();
}

/*
 * Returns the text value of the token in the table (if it exists)
 */
bool sym_table::get_text(const std::string &key, std::string &text) {
	token tok;

	// check if key exists
	if(!contains(key))
		return false;

	// set value
	tok = *table[key];
	text = tok.get_text();
	return true;
}

/*
 * Returns the type value of the token in the table (if it exists)
 */
bool sym_table::get_type(const std::string &key, unsigned int &type) {
	token tok;

	// check if key exists
	if(!contains(key))
		return false;

	// set value
	tok = *table[key];
	type = tok.get_type();
	return true;
}

/*
 * Returns the values of the token in the table (if it exists)
 */
bool sym_table::get_value(const std::string &key, token &value) {
	token tok;

	// check if key exists
	if(!contains(key))
		return false;

	// set value
	tok = *table[key];
	value.set_text(tok.get_text());
	value.set_type(tok.get_type());
	return true;
}

/*
 * Sets value to the values of the token in the table (if it exists)
 */
bool sym_table::set_value(const std::string &key, token &value) {

	// check if key exists
	if(!contains(key))
		table[key] = new token(value.get_text(), value.get_type(), NULL);
		if(!table[key])
			throw exc_code::MEM_FAILURE;
	else {
		table[key]->set_text(value.get_text());
		table[key]->set_type(value.get_type());
	}
	return true;
}

/*
 * Sets value to the values of the token in the table (if it exists)
 */
bool sym_table::set_value(const std::string &key, const std::string &text, unsigned int type) {

	// check if key exists
	if(!contains(key)) {
		table[key] = new token(text, type, NULL);
		if(!table[key])
			throw exc_code::MEM_FAILURE;
	} else {
		table[key]->set_text(text);
		table[key]->set_type(type);
	}
	return true;
}

/*
 * Returns a string representation of the current state of the symbol table
 */
void sym_table::to_string(std::string &str) {
	std::stringstream ss;
	std::map<std::string, token *>::iterator i = table.begin();

	// append all elements in table to string
	if(!empty())
		for(; i != table.end(); ++i) {
			ss.str("");
			ss << i->first << " --> " << i->second->get_text() << std::endl;
			str.append(ss.str());
		}
}
