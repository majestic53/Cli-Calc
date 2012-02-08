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

#include <cmath>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include "calc.hpp"
#include "lexer.hpp"

/*
 * Prompt
 */
const std::string calc::PROMPT(">> ");

/*
 * Copyright statement
 */
const std::string calc::COPYRIGHT("Copyright (C) 2012 David Jolly");

/*
 * Help information
 */
const std::string calc::HELP_INFO_DATA[calc::HELP_INFO_DATA_SIZE] = {
	"about -- print credits",
	"abs -- absolute value",
	"acos -- arc cosine",
	"asin -- arc sine",
	"atan -- arc tangant",
	"ceiling -- ceiling (maintains type)",
	"constants: e, pi",
	"cos -- cosine",
	"cosh -- hyperbolic cosine",
	"fact [n] -- factorial",
	"fib [n] -- fibonacci sequence",
	"float -- cast to floating-point",
	"floor -- floor (maintains type)",
	"int -- cast to integer",
	"ln -- natural log (log-base-e)",
	"log2 -- log-base-2",
	"log10 -- log-base-10",
	"make -- assign an id to an expression",
	"rand -- normalized random numbers (0-1)",
	"reset -- resets the global state",
	"round -- round to nearest integer (maintains type)",
	"sin -- sine",
	"sinh -- hyperbolic sine",
	"sqr -- square",
	"sqrt -- square root",
	"state -- prints the global state",
	"tan -- tangent",
	"tanh -- hyperbolic tangent",
};

/*
 * Help information notification
 */
const std::string calc::NOTIFICATION("Type 'help' or 'about' for more information");

/*
 * Version number
 */
const std::string calc::VERSION("Cli-Calculator 0.1.1");

/*
 * Warranty statement
 */
const std::string calc::WARRANTY("This is free software. There is NO warranty.");

/*
 * Built-in commands
 */
const std::string calc::CMD_DATA[5] = { "about", "exit", "help", "reset", "state" };
const std::set<std::string> calc::CMD_SET(CMD_DATA, CMD_DATA + 5);

/*
 * Command-line commands
 */
const std::string calc::C_CMD_DATA[3] = { "--help", "--version" };
const std::set<std::string> calc::C_CMD_SET(C_CMD_DATA, C_CMD_DATA + 2);

/*
 * Checks input for commands prior to evaluation
 */
int calc::check_input(std::string &input, sym_table &state) {
	std::string str;
	std::vector<std::string> commands;

	// parse input line for commands
	calc::get_commands(input, commands);
	if(commands.empty())
		return exc_code::SUCCESS;

	// parse input as a command
	if(calc::CMD_SET.find(commands.at(0)) != calc::CMD_SET.end()) {

		// display about information
		if(commands.at(0) == calc::CMD_DATA[calc::ABOUT])
			std::cout << calc::VERSION << " -- " << calc::COPYRIGHT << std::endl << calc::WARRANTY << std::endl;

		// exit interactive mode
		else if(commands.at(0) == calc::CMD_DATA[calc::EXIT])
			return exc_code::EXIT;

		// display help information
		else if(commands.at(0) == calc::CMD_DATA[calc::HELP])
			for(unsigned int i = 0; i < calc::HELP_INFO_DATA_SIZE; i++)
				std::cout << calc::HELP_INFO_DATA[i] << std::endl;

		// reset global state
		else if(commands.at(0) == calc::CMD_DATA[calc::RESET])
			state.cleanup();

		// diplay global state
		else if(commands.at(0) == calc::CMD_DATA[calc::STATE]) {
			state.to_string(str);
			std::cout << str;

		// unknown command
		} else
			std::cerr << "Unknown command: " << commands.at(0) << std::endl;

	// evaluate input
	} else
		return calc::eval_input(input, state);
	return exc_code::SUCCESS;
}

/*
 * Evaluate a constant
 */
void calc::eval_constant(syn_tree &tree) {
	token tok;
	std::string output;

	// verify token is of type constant
	tree.get_contents(tok);
	if(tok.get_type() != token::CONSTANT)
		throw exc_code::INVALID_CONSTANT;

	// evaluate as exp
	if(tok.get_text() == lexer::CONSTANT_OPER_DATA[lexer::E]) {
		token::convert_to_string((double) exp(1), output);
		tree.set_text(output);
		tree.set_type(token::FLOAT);

	// evaluate as pi
	} else if(tok.get_text() == lexer::CONSTANT_OPER_DATA[lexer::PI]) {
		token::convert_to_string((double) (std::atan(1.0) * 4.0), output);
		tree.set_text(output);
		tree.set_type(token::FLOAT);

	// evaluate as a random number between 0.0 - 1.0
	} else if(tok.get_text() == lexer::CONSTANT_OPER_DATA[lexer::RAND]) {
		token::convert_to_string((double) (rand() / (double) RAND_MAX), output);
		tree.set_text(output);
		tree.set_type(token::FLOAT);
	} else
		throw exc_code::INVALID_CONSTANT;
}

/*
 * Evaluate an expression
 */
void calc::eval_expression(syn_tree &tree, sym_table &state) {
	token accum, child;
	unsigned int type, size;

	// verify token is of type expression
	tree.get_type(type);
	if(type != token::EXPRESSION)
		throw exc_code::INVALID_EXPRESSION;

	// evaluate child tokens left to right
	size = tree.get_size();
	for(unsigned int i = 0; i < size; i++) {
		tree.advance_forward(i);
		tree.get_contents(child);

		// evaluate first token
		if(!i) {
			switch(child.get_type()) {

				// evaluate as a constant
				case token::CONSTANT: eval_constant(tree);
					break;

				// evaluate as an expression
				case token::EXPRESSION: eval_expression(tree, state);
					break;

				// evaluate as a function
				case token::FUNCTION: eval_function(tree, state);
					break;

				// evaluate as a string
				case token::STRING:
					if(!state.get_value(child.get_text(), accum))
						throw exc_code::UNDEFINED_IDENTIFIER;
					tree.set_contents(accum);
					break;

				// evaluate as a unary operator
				case token::UNARY_OPER:
					if(child.get_text() == lexer::UNARY_OPER_DATA[lexer::NOT]) {
						tree.advance_forward(i);
						eval_expression(tree, state);
						tree.negate();
						tree.get_contents(accum);
						tree.advance_back();
						tree.set_contents(accum);
					} else
						throw exc_code::INVALID_UNARY_OPERATOR;
					break;
				default:
					if(child.get_type() != token::INTEGER
							&& child.get_type() != token::FLOAT)
						throw exc_code::INVALID_EXPRESSION;
					break;
			}
			tree.get_contents(accum);

		// evaluate second token
		} else {
			if(tree.get_size() != 1
					|| (child.get_type() != token::BINARY_OPER
					&& child.get_type() != token::LOGICAL_OPER
					&& child.get_type() != token::OPER))
				throw exc_code::INVALID_EXPRESSION;

			// evaluate as an operation
			eval_operator(tree, accum, state);
		}

		// set the contents of the accumulator token (root)
		tree.advance_back();
		tree.set_contents(accum);
	}
}

/*
 * Evaluate a function
 */
void calc::eval_function(syn_tree &tree, sym_table &state) {
	token tok, child;
	unsigned int type;
	std::string text, output;

	// verify token is of type function
	tree.get_type(type);
	if(type != token::FUNCTION
			|| tree.get_size() != 1)
		throw exc_code::INVALID_FUNCTION;

	// Retrieve child value
	tree.get_text(text);
	tree.advance_forward(0);
	eval_expression(tree, state);
	tree.get_contents(child);
	tree.advance_back();

	// check that child type is valid
	if(child.get_type() != token::INTEGER
			&& child.get_type() != token::FLOAT)
		throw exc_code::INVALID_FUNCTION;

	// execute abs function on input
	if(text == lexer::FUNCTION_OPER_DATA[lexer::ABS]) {
		tree.set_type(child.get_type());
		if(child.get_type() == token::INTEGER)
			token::convert_to_string(std::abs(token::convert_to<long>(child.get_text())), output);
		else;
			token::convert_to_string(fabs(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);

	// execute arc cos function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::ACOS]) {
		tree.set_type(token::FLOAT);
		token::convert_to_string(acos(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);

	// execute arc sin function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::ASIN]) {
		tree.set_type(token::FLOAT);
		token::convert_to_string(asin(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);

	// execute arc tan function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::ATAN]) {
		tree.set_type(token::FLOAT);
		token::convert_to_string(atan(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);

	// execute ceiling function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::CEILING]) {
		tree.set_type(child.get_type());
		if(child.get_type() == token::INTEGER)
			token::convert_to_string(ceil(token::convert_to<long>(child.get_text())), output);
		else
			token::convert_to_string(ceil(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);

	// execute cos function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::COS]) {
		tree.set_type(token::FLOAT);
		token::convert_to_string(cos(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);

	// execute hyperbolic cos function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::COSH]) {
		tree.set_type(token::FLOAT);
		token::convert_to_string(cosh(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);

	// execute factorial function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::FACT]) {
		tree.set_type(token::INTEGER);

		// check that operand is valid
		if(child.get_type() != token::INTEGER)
			throw exc_code::EXPECTING_INTEGER_OPERAND;
		long start = token::convert_to<long>(child.get_text());

		// check for positive value
		if(start < 0)
			throw exc_code::EXPECTING_POSITIVE_INTEGER_OPERAND;

		// calculate factorial
		if(start < 1)
			token::convert_to_string((long) 1, output);
		else {
			long prod = 1;
			for(long i = start; i > 1; --i)
				prod *= i;
			token::convert_to_string(prod, output);
		}
		tree.set_text(output);

	// execute fibonacci function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::FIB]) {
		tree.set_type(token::INTEGER);

		// check that operand is valid
		if(child.get_type() != token::INTEGER)
			throw exc_code::EXPECTING_INTEGER_OPERAND;
		long start = token::convert_to<long>(child.get_text());

		// check for positive value
		if(start < 0)
			throw exc_code::EXPECTING_POSITIVE_INTEGER_OPERAND;

		// calculate fibonacci value
		if(start < 2)
			token::convert_to_string(start, output);
		else {
			long *fib = new long[start + 1];
			if(!fib)
				throw exc_code::MEM_FAILURE;
			fib[0] = 0;
			fib[1] = 1;
			for(long i = 2; i <= start; i++)
				fib[i] = fib[i - 1] + fib[i - 2];
			token::convert_to_string(fib[start], output);
			delete[] fib;
		}
		tree.set_text(output);

	// execute double cast function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::FLOAT]) {
		tree.set_type(token::FLOAT);
		token::convert_to_string(token::convert_to<double>(child.get_text()), output);
		tree.set_text(output);

	// execute floor function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::FLOOR]) {
		tree.set_type(child.get_type());
		if(child.get_type() == token::INTEGER)
			token::convert_to_string(floor(token::convert_to<long>(child.get_text())), output);
		else
			token::convert_to_string(floor(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);

	// execute integer cast function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::INT]) {
		tree.set_type(token::INTEGER);
		token::convert_to_string(token::convert_to<long>(child.get_text()), output);
		tree.set_text(output);

	// execute natural log function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::LN]) {
		tree.set_type(token::FLOAT);
		token::convert_to_string(log(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);


	// execute log(base 2) function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::LOG2]) {
		tree.set_type(token::FLOAT);
		token::convert_to_string(log2(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);

	// execute log(base 10) function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::LOG10]) {
		tree.set_type(token::FLOAT);
		token::convert_to_string(log10(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);

	// execute round function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::ROUND]) {
		tree.set_type(child.get_type());
		if(child.get_type() == token::INTEGER)
			token::convert_to_string(round(token::convert_to<long>(child.get_text())), output);
		else
			token::convert_to_string(round(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);

	// execute sin function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::SIN]) {
		tree.set_type(token::FLOAT);
		token::convert_to_string(sin(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);

	// execute hyperbolic sin function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::SINH]) {
		tree.set_type(token::FLOAT);
		token::convert_to_string(sinh(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);

	// execute square function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::SQR]) {
		tree.set_type(token::FLOAT);
		if(child.get_type() == token::INTEGER) {
			long value = token::convert_to<long>(child.get_text());
			token::convert_to_string(value * value, output);
		} else {
			double value = token::convert_to<double>(child.get_text());
			token::convert_to_string(value * value, output);
		}
		tree.set_text(output);

	// execute sqrt function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::SQRT]) {
		tree.set_type(token::FLOAT);
		token::convert_to_string(sqrt(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);

	// execute tan function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::TAN]) {
		tree.set_type(token::FLOAT);
		token::convert_to_string(tan(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);

	// execute hyperbolic tan function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::TANH]) {
		tree.set_type(token::FLOAT);
		token::convert_to_string(tanh(token::convert_to<double>(child.get_text())), output);
		tree.set_text(output);
	} else
		throw exc_code::INVALID_FUNCTION;
}

/*
 * Evaluate an operator
 */
void calc::eval_operator(syn_tree &tree, token &accum, sym_table &state) {
	unsigned int type;
	token second, oper;
	std::string output;

	// check to make sure accum is a valid type
	if(accum.get_type() != token::INTEGER
			&& accum.get_type() != token::FLOAT)
		throw exc_code::INVALID_OPERAND;

	// retrieve second operand & operator
	tree.get_contents(oper);
	tree.advance_forward(0);
	eval_expression(tree, state);
	tree.get_contents(second);
	tree.advance_back();

	// check to make sure second is a valid type
	if(second.get_type() != token::INTEGER
			&& second.get_type() != token::FLOAT) {
		std::string str;
		second.to_string(str);
		std::cout << str << std::endl;
		throw exc_code::INVALID_OPERAND;
	}

	// determine type
	type = token::INTEGER;
	if(accum.get_type() != token::INTEGER
			|| second.get_type() != token::INTEGER)
		type = token::FLOAT;

	// evaluate based off operator type
	switch(oper.get_type()) {

		// evaluate as a binary operator
		case token::BINARY_OPER:

			// evaluate as a binary and
			if(oper.get_text() == lexer::BINARY_OPER_DATA[lexer::AND]) {
				if(type != token::INTEGER)
					throw exc_code::EXPECTING_INTEGER_OPERAND;
				token::convert_to_string((long) token::convert_to<long>(accum.get_text()) & token::convert_to<long>(second.get_text()), output);

			// evaluate as a binary or
			} else if(oper.get_text() == lexer::BINARY_OPER_DATA[lexer::OR]) {
				if(type != token::INTEGER)
					throw exc_code::EXPECTING_INTEGER_OPERAND;
				token::convert_to_string((long) token::convert_to<long>(accum.get_text()) | token::convert_to<long>(second.get_text()), output);

			// evaluate as a binary xor
			} else if(oper.get_text() == lexer::BINARY_OPER_DATA[lexer::XOR]) {
				if(type != token::INTEGER)
					throw exc_code::EXPECTING_INTEGER_OPERAND;
				token::convert_to_string((long) token::convert_to<long>(accum.get_text()) ^ token::convert_to<long>(second.get_text()), output);

			} else
				throw exc_code::INVALID_BINARY_OPERATOR;
			break;

		// evaluate as a logical operator
		case token::LOGICAL_OPER:

			// evaluate as a logical left shift
			if(oper.get_text() == lexer::LOGICAL_OPER_DATA[lexer::LEFT_SHIFT]) {
				if(type != token::INTEGER)
					throw exc_code::EXPECTING_INTEGER_OPERAND;
				token::convert_to_string((long) token::convert_to<long>(accum.get_text()) << token::convert_to<long>(second.get_text()), output);

			// evaluate as a logical right shift
			} else if(oper.get_text() == lexer::LOGICAL_OPER_DATA[lexer::RIGHT_SHIFT]) {
				if(type != token::INTEGER)
					throw exc_code::EXPECTING_INTEGER_OPERAND;
				token::convert_to_string((long) token::convert_to<long>(accum.get_text()) >> token::convert_to<long>(second.get_text()), output);

			} else
				throw exc_code::INVALID_LOGICAL_OPERATOR;
			break;

		// evaluate as an arithmetic operator
		case token::OPER:

			// evaluate as a arithmetic plus
			if(oper.get_text() == lexer::OPER_DATA[lexer::PLUS]) {
				if(type == token::INTEGER)
					token::convert_to_string((long) token::convert_to<long>(accum.get_text()) + token::convert_to<long>(second.get_text()), output);
				else
					token::convert_to_string((double) token::convert_to<double>(accum.get_text()) + token::convert_to<double>(second.get_text()), output);

			// evaluate as a arithmetic minus
			} else if(oper.get_text() == lexer::OPER_DATA[lexer::MINUS]) {
				if(type == token::INTEGER)
					token::convert_to_string((long) token::convert_to<long>(accum.get_text()) - token::convert_to<long>(second.get_text()), output);
				else
					token::convert_to_string((double) token::convert_to<double>(accum.get_text()) - token::convert_to<double>(second.get_text()), output);

			// evaluate as a arithmetic multiply
			} else if(oper.get_text() == lexer::OPER_DATA[lexer::MULTI]) {
				if(type == token::INTEGER)
					token::convert_to_string((long) token::convert_to<long>(accum.get_text()) * token::convert_to<long>(second.get_text()), output);
				else
					token::convert_to_string((double) token::convert_to<double>(accum.get_text()) * token::convert_to<double>(second.get_text()), output);

			// evaluate as a arithmetic divide
			} else if(oper.get_text() == lexer::OPER_DATA[lexer::DIV]) {
				if(type == token::INTEGER)
					token::convert_to_string((long) token::convert_to<long>(accum.get_text()) / token::convert_to<long>(second.get_text()), output);
				else
					token::convert_to_string((double) token::convert_to<double>(accum.get_text()) / token::convert_to<double>(second.get_text()), output);

			// evaluate as a arithmetic modulo
			} else if(oper.get_text() == lexer::OPER_DATA[lexer::MOD]) {
				if(type != token::INTEGER)
					throw exc_code::EXPECTING_INTEGER_OPERAND;
				token::convert_to_string((long) token::convert_to<long>(accum.get_text()) % token::convert_to<long>(second.get_text()), output);

			// evaluate as a arithmetic power
			} else if(oper.get_text() == lexer::OPER_DATA[lexer::POW]) {
				if(type == token::INTEGER)
					token::convert_to_string((long) pow(token::convert_to<long>(accum.get_text()), token::convert_to<long>(second.get_text())), output);
				else
					token::convert_to_string((double) pow(token::convert_to<double>(accum.get_text()), token::convert_to<double>(second.get_text())), output);
			} else
				throw exc_code::INVALID_ARITHMETIC_OPERATOR;
			break;
		default:
			throw exc_code::INVALID_OPERATOR;
			break;
	}
	accum.set_type(type);
	accum.set_text(output);
}

/*
 * Evaluates a given input string and state
 */
int calc::eval_input(std::string &input, sym_table &state) {
	token tok;
	parser par;
	std::string output;
	syn_tree copy, *curr = NULL;
	std::vector<syn_tree *>::iterator i;
	unsigned int root_type;

	try {

		// parse input
		par = parser(input);
		par.parse();
		std::vector<syn_tree *> tree = par.get_syntax_tree();
		i = tree.begin();

		// iterate through trees
		for(; i != tree.end(); ++i) {
			curr = *i;
			curr->advance_root();
			curr->get_type(root_type);
			output.clear();

			// TODO: for debugging (remove)
			/*std::string str;
			syn_tree::print_tree(**i, str);
			std::cout << str << std::endl;*/

			// evaluate based off root token type
			switch(root_type) {

				// evaluate as an assignment
				case token::ASSIGNMENT:
					curr->get_child_text(output, 0);

					// evaulate assignment expression
					curr->advance_forward(1);
					eval_expression(*curr, state);
					curr->get_contents(tok);

					// set value
					state.set_value(output, tok);
					break;

				// evaluate as an expression
				case token::EXPRESSION:

					// evaluate expression
					eval_expression(*curr, state);
					curr->advance_root();
					curr->get_text(output);

					// print output
					if(output.empty())
						continue;
					std::cout << output << std::endl;
					break;

				default: throw exc_code::INVALID_EXPRESSION;
					break;
			}
		}

		// release resources
		par.cleanup();
	} catch(int e) {

		// catch exceptions
		std::string show_exc(input);
		show_exc.insert(par.get_position() - 1, "[ ]");
		std::cerr << "Exception (" << e << "): " << show_exc << " (" << exc_code::MESSAGE[e] << ")" << std::endl;
		par.cleanup();
		return e;
	}
	return exc_code::SUCCESS;
}

/*
 * Returns a series of individual commands parsed from input
 */
int calc::get_commands(const std::string &input, std::vector<std::string> &commands) {
	char ch;
	unsigned int pos = 0;
	std::string command;

	// return if input is empty
	if(input.empty())
		return 0;

	// remove all whitespace preceeding the first command
	ch = input.at(pos);
	while(isspace(ch)) {
		pos++;
		ch = input.at(pos);
	}

	// parse the entire contents of the input
	while(pos < input.size()) {
		ch = input.at(pos);
		if(isspace(ch))
			if(command.empty()) {
				pos++;
				continue;
			} else {
				commands.push_back(command);
				command.clear();
			}
		else
			command += ch;
		pos++;
	}

	// catch remaining commands
	if(!command.empty())
		commands.push_back(command);

	// return the number of commands parsed
	return commands.size();
}

/*
 * Main
 */
int main(int argc, char *argv[]) {
	srand(time(NULL));
	std::string input;
	std::vector<std::string> commands;
	std::vector<std::string>::iterator cmd;
	bool prompt = false;
	long exit_code = exc_code::SUCCESS;
	sym_table state;

	// if arguments are given read them in as input
	if(argc > 1) {
		bool run_input = true;
		for(int i = 1; i < argc; i++) {
			input = argv[i];

			// parse input as a command-line command
			if(calc::C_CMD_SET.find(argv[i]) != calc::C_CMD_SET.end()) {
				run_input = false;
				if(input == calc::C_CMD_DATA[calc::C_HELP]) {
					std::cout << calc::VERSION << " -- " << calc::COPYRIGHT << std::endl << calc::WARRANTY << std::endl << std::endl;
					std::cout << calc::C_CMD_DATA[calc::C_HELP] << "\t\tDisplay help information" << std::endl << std::endl;
					std::cout << calc::C_CMD_DATA[calc::C_VERSION] << "\tDisplay version information" << std::endl << std::endl;
					std::cout << "If no input is given, set to interactive mode, otherwise" << std::endl;
					std::cout << "expressions will be evaluated in order that they appear." << std::endl << std::endl;
				} else if(input == calc::C_CMD_DATA[calc::C_VERSION])
					std::cout << calc::VERSION << " -- " << calc::COPYRIGHT << std::endl << calc::WARRANTY << std::endl << std::endl;
				else
					std::cerr << "Unknown command: " << input << std::endl;
				break;
			} else
				commands.push_back(input);
		}

		// run command-line input, if appropriate
		if(run_input
				&& commands.size())
			for(cmd = commands.begin(); cmd != commands.end(); ++cmd)
				exit_code = calc::check_input(*cmd, state);

	// else, enter interactive-mode
	} else {

		// trap ctrl^c keyboard interrupt
		std::signal(SIGINT, calc::keyboard_interrupt0);

		// initialize prompt
		prompt = true;
		std::cout << calc::VERSION << " -- " << calc::COPYRIGHT << std::endl << calc::NOTIFICATION << std::endl;

		// while exit command is not issued except input
		while(prompt) {

			// if stdin pipe is closed, exit
			if(!std::cin) {
				exit_code = exc_code::STDIN_EOF;
				break;
			}

			// grab input line from user
			input.clear();
			std::cout << calc::PROMPT;
			std::getline(std::cin, input);

			// if input is empty, continue
			if(input.empty())
				continue;

			// run input
			if((exit_code = calc::check_input(input, state)) == exc_code::EXIT)
				break;
		}
	}

	// release resources
	state.cleanup();
	return exit_code;
}
