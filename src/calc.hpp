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

#ifndef CALC_HPP_
#define CALC_HPP_

#include <string>
#include <vector>
#include "exc_code.hpp"
#include "parser.hpp"
#include "sym_table.hpp"
#include "syn_tree.hpp"
#include "token.hpp"

class calc {
public:

	/*
	 * Prompt
	 */
	static const std::string PROMPT;

	/*
	 * Copyright statement
	 */
	static const std::string COPYRIGHT;

	/*
	 * Help information
	 */
	static const std::string HELP_INFO_DATA[];
	static const unsigned int HELP_INFO_DATA_SIZE = 27;

	/*
	 * Help information notification
	 */
	static const std::string NOTIFICATION;

	/*
	 * Version number
	 */
	static const std::string VERSION;

	/*
	 * Warranty statement
	 */
	static const std::string WARRANTY;

	/*
	 * Built-in commands
	 */
	enum CMD { ABOUT, EXIT, HELP, RESET, STATE };
	static const std::string CMD_DATA[];
	static const std::set<std::string> CMD_SET;

	/*
	 * Command-line commands
	 */
	enum C_CMD { C_HELP, C_VERSION };
	static const std::string C_CMD_DATA[];
	static const std::set<std::string> C_CMD_SET;

	/*
	 * Checks input for commands prior to evaluation
	 */
	static int check_input(std::string &input, sym_table &state);

	/*
	 * Evaluate a constant
	 */
	static void eval_constant(syn_tree &tree);

	/*
	 * Evaluate an expression
	 */
	static void eval_expression(syn_tree &tree, sym_table &state);

	/*
	 * Evaluate a function
	 */
	static void eval_function(syn_tree &tree, sym_table &state);

	/*
	 * Evaluate an operator
	 */
	static void eval_operator(syn_tree &tree, token &accum, sym_table &state);

	/*
	 * Evaluates a given input string and state
	 */
	static int eval_input(std::string &input, sym_table &state);

	/*
	 * Returns a series of individual commands parsed from input
	 */
	static int get_commands(const std::string &input, std::vector<std::string> &commands);

	/*
	 * Handle Ctrl^C keyboard interrupts
	 */
	static void keyboard_interrupt0(int sig) { return; }
};

#endif
