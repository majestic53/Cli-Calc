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

#ifndef SYM_TABLE_HPP_
#define SYM_TABLE_HPP_

#include <map>
#include <string>
#include "token.hpp"

class sym_table {
private:

	std::map<std::string, token *> table;

public:

	/*
	 * Symbol table constructor
	 */
	sym_table(void) { table.clear(); }

	/*
	 * Symbol table constructor
	 */
	sym_table(const sym_table &other) { table = other.table; }

	/*
	 * Symbol table constructor
	 */
	sym_table(std::map<std::string, token *> &table) { this->table = table; }

	/*
	 * Symbol table destructor
	 */
	~sym_table(void) { cleanup(); }

	/*
	 * Symbol table assignment
	 */
	sym_table &operator=(const sym_table &other);

	/*
	 * Symbol table equivalence
	 */
	bool operator==(const sym_table &other);

	/*
	 * Symbol table equivalence
	 */
	bool operator!=(const sym_table &other) { return !(*this == other); }

	/*
	 * Cleanup resources used by symbol table
	 */
	void cleanup(void);

	/*
	 * Test if key exists within table
	 */
	bool contains(const std::string &key) { return table.find(key) != table.end(); }

	/*
	 * Returns whether table is empty
	 */
	bool empty(void) { return table.empty(); }

	/*
	 * Returns the text value of the token in the table (if it exists)
	 */
	bool get_text(const std::string &key, std::string &text);

	/*
	 * Returns the type value of the token in the table (if it exists)
	 */
	bool get_type(const std::string &key, unsigned int &type);

	/*
	 * Returns the values of the token in the table (if it exists)
	 */
	bool get_value(const std::string &key, token &value);

	/*
	 * Sets the text value to the values of the token in the table (if it exists)
	 */
	bool set_text(const std::string &key, std::string &text) { return set_value(key, text, token::UNDEFINED); }

	/*
	 * Sets the type value to the values of the token in the table (if it exists)
	 */
	bool set_type(const std::string &key, unsigned int type) { return set_value(key, "", type); }

	/*
	 * Sets value to the values of the token in the table (if it exists)
	 */
	bool set_value(const std::string &key, token &value);

	/*
	 * Sets value to the values of the token in the table (if it exists)
	 */
	bool set_value(const std::string &key, const std::string &text, unsigned int type);

	/*
	 * Returns the size of the table
	 */
	unsigned int size(void) { return table.size(); }

	/*
	 * Returns a string representation of the current state of the symbol table
	 */
	void to_string(std::string &str);
};

#endif /* SYM_TABLE_HPP_ */
