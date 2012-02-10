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
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include "calc.hpp"
#include "lexer.hpp"

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
	"atan -- arc tangent",
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
 * Prompt
 */
const std::string calc::PROMPT(">> ");

/*
 * Version number
 */
const std::string calc::VERSION("Cli-Calculator 0.1.2");

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
 * Convert mpfr_t to string
 */
bool calc::convert_to_string(mpfr_t &value, const unsigned int precision, std::string &output) {
	char *tmp = NULL;
	mp_exp_t dec_pos;

	// convert to a string
	tmp = mpfr_get_str(tmp, &dec_pos, 10, precision, value, GMP_RNDN);
	if(!tmp)
		return false;
	else {
		output.assign(tmp);
		if(dec_pos <= 0) {
			if(output.at(0) == '-')
				output.insert(1, "0.");
			else
				output.insert(0, "0.");
		} else if((unsigned int) dec_pos < output.size())
			output.insert(dec_pos, ".");
	}
	mpfr_free_str(tmp);
	return true;
}

/*
 * Evaluate a constant
 */
void calc::eval_constant(syn_tree &tree) {
	token tok;
	mpfr_t value;
	std::string output;
	mpfr_init(value);

	// verify token is of type constant
	tree.get_contents(tok);
	if(tok.get_type() != token::CONSTANT)
		throw exc_code::INVALID_CONSTANT;

	// evaluate as exp
	if(tok.get_text() == lexer::CONSTANT_OPER_DATA[lexer::E]) {
		mpfr_t one;
		mpfr_init_set_ui(one, 1, GMP_RNDN);
		mpfr_exp(value, one, GMP_RNDN);
		mpfr_clear(one);
	}

	// evaluate as pi
	else if(tok.get_text() == lexer::CONSTANT_OPER_DATA[lexer::PI])
		mpfr_const_pi(value, GMP_RNDN);

	// evaluate as a random number between 0.0 - 1.0
	else if(tok.get_text() == lexer::CONSTANT_OPER_DATA[lexer::RAND]) {
		gmp_randstate_t rand_st;
		gmp_randinit_mt(rand_st);
		gmp_randseed_ui(rand_st, time(NULL));
		mpfr_urandomb(value, rand_st);
		gmp_randclear(rand_st);
	}

	else
		throw exc_code::INVALID_CONSTANT;

	// convert value to string
	if(!convert_to_string(value, 0, output))
		output = "0.0";

	tree.set_text(output);
	tree.set_type(token::FLOAT);

	// cleanup
	mpfr_clear(value);
	mpfr_free_cache();
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
	tree.set_type(token::FLOAT);

	// execute abs function on input
	if(text == lexer::FUNCTION_OPER_DATA[lexer::ABS]) {
		if(child.get_type() == token::INTEGER) {
			tree.set_type(token::INTEGER);
			mpz_t value;
			token::convert_to_integer(value, child.get_text());
			mpz_abs(value, value);
			token::convert_to_string(value, output);
			mpz_clear(value);
		} else {
			mpf_t value;
			token::convert_to_float(value, child.get_text());
			mpf_abs(value, value);
			token::convert_to_string(value, output);
			mpf_clear(value);
		}

	// execute arc cos function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::ACOS]) {
		mpfr_t value, input;
		mpfr_init(value);
		mpfr_init_set_str(input, child.get_text().c_str(), 10, GMP_RNDN);
		mpfr_acos(value, input, GMP_RNDN);

		// convert value to string
		if(!convert_to_string(value, 0, output))
			output = "0.0";
		mpfr_clear(value);
		mpfr_clear(input);

	// execute arc sin function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::ASIN]) {
		mpfr_t value, input;
		mpfr_init(value);
		mpfr_init_set_str(input, child.get_text().c_str(), 10, GMP_RNDN);
		mpfr_asin(value, input, GMP_RNDN);

		// convert value to string
		if(!convert_to_string(value, 0, output))
			output = "0.0";
		mpfr_clear(value);
		mpfr_clear(input);

	// execute arc tan function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::ATAN]) {
		mpfr_t value, input;
		mpfr_init(value);
		mpfr_init_set_str(input, child.get_text().c_str(), 10, GMP_RNDN);
		mpfr_atan(value, input, GMP_RNDN);

		// convert value to string
		if(!convert_to_string(value, 0, output))
			output = "0.0";
		mpfr_clear(value);
		mpfr_clear(input);

	// execute ceiling function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::CEILING]) {
		if(child.get_type() != token::INTEGER) {
			mpz_t i_val;
			mpf_t f_val;
			mpz_init(i_val);
			token::convert_to_float(f_val, child.get_text());
			mpz_set_f(i_val, f_val);
			mpf_set_z(f_val, i_val);
			mpf_add_ui(f_val, f_val, 1);
			token::convert_to_string(f_val, output);
			mpz_clear(i_val);
			mpf_clear(f_val);
		} else {
			tree.set_type(token::INTEGER);
			output = child.get_text();
		}

	// execute cos function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::COS]) {
		mpfr_t value, input;
		mpfr_init(value);
		mpfr_init_set_str(input, child.get_text().c_str(), 10, GMP_RNDN);
		mpfr_cos(value, input, GMP_RNDN);

		// convert value to string
		if(!convert_to_string(value, 0, output))
			output = "0.0";
		mpfr_clear(value);
		mpfr_clear(input);

	// execute hyperbolic cos function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::COSH]) {
		mpfr_t value, input;
		mpfr_init(value);
		mpfr_init_set_str(input, child.get_text().c_str(), 10, GMP_RNDN);
		mpfr_cosh(value, input, GMP_RNDN);

		// convert value to string
		if(!convert_to_string(value, 0, output))
			output = "0.0";
		mpfr_clear(value);
		mpfr_clear(input);

	// execute factorial function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::FACT]) {
		if(child.get_type() == token::INTEGER) {
			tree.set_type(token::INTEGER);
			mpz_t value;
			mpz_init(value);
			uint64_t sec;
			sec = token::convert_to<uint64_t>(child.get_text());
			mpz_fac_ui(value, sec);
			token::convert_to_string(value, output);
			mpz_clear(value);
		} else
			throw exc_code::EXPECTING_INTEGER_OPERAND;

	// execute fibonacci function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::FIB]) {
		if(child.get_type() == token::INTEGER) {
			tree.set_type(token::INTEGER);
			mpz_t value;
			mpz_init(value);
			uint64_t sec;
			sec = token::convert_to<uint64_t>(child.get_text());
			mpz_fib_ui(value, sec);
			token::convert_to_string(value, output);
			mpz_clear(value);
		} else
			throw exc_code::EXPECTING_INTEGER_OPERAND;

	// execute double cast function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::FLOAT]) {
		output = child.get_text();

	// execute floor function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::FLOOR]) {
		if(child.get_type() != token::INTEGER) {
			mpz_t i_val;
			mpf_t f_val;
			mpz_init(i_val);
			token::convert_to_float(f_val, child.get_text());
			mpz_set_f(i_val, f_val);
			mpf_set_z(f_val, i_val);
			token::convert_to_string(f_val, output);
			mpz_clear(i_val);
			mpf_clear(f_val);
		} else {
			tree.set_type(token::INTEGER);
			output = child.get_text();
		}

	// execute integer cast function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::INT]) {
		if(child.get_type() != token::INTEGER) {
			tree.set_type(token::INTEGER);
			mpz_t i_val;
			mpf_t f_val;
			mpz_init(i_val);
			token::convert_to_float(f_val, child.get_text());
			mpz_set_f(i_val, f_val);
			token::convert_to_string(i_val, output);
			mpz_clear(i_val);
			mpf_clear(f_val);
		} else {
			tree.set_type(token::INTEGER);
			output = child.get_text();
		}

	// execute natural log function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::LN]) {
		mpfr_t value, input;
		mpfr_init(value);
		mpfr_init_set_str(input, child.get_text().c_str(), 10, GMP_RNDN);
		mpfr_log(value, input, GMP_RNDN);

		// convert value to string
		if(!convert_to_string(value, 0, output))
			output = "0.0";
		mpfr_clear(value);
		mpfr_clear(input);

	// execute log(base 2) function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::LOG2]) {
		mpfr_t value, input;
		mpfr_init(value);
		mpfr_init_set_str(input, child.get_text().c_str(), 10, GMP_RNDN);
		mpfr_log2(value, input, GMP_RNDN);

		// convert value to string
		if(!convert_to_string(value, 0, output))
			output = "0.0";
		mpfr_clear(value);
		mpfr_clear(input);

	// execute log(base 10) function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::LOG10]) {
		mpfr_t value, input;
		mpfr_init(value);
		mpfr_init_set_str(input, child.get_text().c_str(), 10, GMP_RNDN);
		mpfr_log10(value, input, GMP_RNDN);

		// convert value to string
		if(!convert_to_string(value, 0, output))
			output = "0.0";
		mpfr_clear(value);
		mpfr_clear(input);

	// execute round function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::ROUND]) {
		if(child.get_type() != token::INTEGER) {
			mpz_t floor;
			mpf_t f_diff, f_floor;
			mpz_init(floor);
			mpf_init(f_floor);
			token::convert_to_float(f_diff, child.get_text());
			mpz_set_f(floor, f_diff);
			mpf_set_z(f_floor, floor);
			mpf_sub(f_diff, f_diff, f_floor);
			if(mpf_cmp_d(f_diff, 0.5) >= 0)
				mpf_add_ui(f_floor, f_floor, 1);
			token::convert_to_string(f_floor, output);
			mpz_clear(floor);
			mpf_clear(f_diff);
			mpf_clear(f_floor);
		} else {
			tree.set_type(token::INTEGER);
			output = child.get_text();
		}

	// execute sin function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::SIN]) {
		mpfr_t value, input;
		mpfr_init(value);
		mpfr_init_set_str(input, child.get_text().c_str(), 10, GMP_RNDN);
		mpfr_sin(value, input, GMP_RNDN);

		// convert value to string
		if(!convert_to_string(value, 0, output))
			output = "0.0";
		mpfr_clear(value);
		mpfr_clear(input);

	// execute hyperbolic sin function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::SINH]) {
		mpfr_t value, input;
		mpfr_init(value);
		mpfr_init_set_str(input, child.get_text().c_str(), 10, GMP_RNDN);
		mpfr_sinh(value, input, GMP_RNDN);

		// convert value to string
		if(!convert_to_string(value, 0, output))
			output = "0.0";
		mpfr_clear(value);
		mpfr_clear(input);

	// execute square function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::SQR]) {
		if(child.get_type() == token::INTEGER) {
			tree.set_type(token::INTEGER);
			mpz_t value;
			token::convert_to_integer(value, child.get_text());
			mpz_mul(value, value, value);
			token::convert_to_string(value, output);
			mpz_clear(value);
		} else {
			mpf_t value;
			token::convert_to_float(value, child.get_text());
			mpf_mul(value, value, value);
			token::convert_to_string(value, output);
			mpf_clear(value);
		}

	// execute sqrt function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::SQRT]) {
		mpf_t value;
		token::convert_to_float(value, child.get_text());
		mpf_sqrt(value, value);
		token::convert_to_string(value, output);
		mpf_clear(value);

	// execute tan function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::TAN]) {
		mpfr_t value, input;
		mpfr_init(value);
		mpfr_init_set_str(input, child.get_text().c_str(), 10, GMP_RNDN);
		mpfr_tan(value, input, GMP_RNDN);

		// convert value to string
		if(!convert_to_string(value, 0, output))
			output = "0.0";
		mpfr_clear(value);
		mpfr_clear(input);

	// execute hyperbolic tan function on input
	} else if(text == lexer::FUNCTION_OPER_DATA[lexer::TANH]) {
		mpfr_t value, input;
		mpfr_init(value);
		mpfr_init_set_str(input, child.get_text().c_str(), 10, GMP_RNDN);
		mpfr_tanh(value, input, GMP_RNDN);

		// convert value to string
		if(!convert_to_string(value, 0, output))
			output = "0.0";
		mpfr_clear(value);
		mpfr_clear(input);

	} else
		throw exc_code::INVALID_FUNCTION;
	tree.set_text(output);
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
			&& second.get_type() != token::FLOAT)
		throw exc_code::INVALID_OPERAND;

	// determine type
	type = token::INTEGER;
	if(accum.get_type() != token::INTEGER
			|| second.get_type() != token::INTEGER)
		type = token::FLOAT;

	// evaluate based off operator type
	switch(oper.get_type()) {

		// evaluate as a binary operator
		case token::BINARY_OPER:

			// check for correct type
			if(type == token::INTEGER) {
				mpz_t value, sec;
				token::convert_to_integer(value, accum.get_text());
				token::convert_to_integer(sec, second.get_text());

				// evaluate as a binary and
				if(oper.get_text() == lexer::BINARY_OPER_DATA[lexer::AND])
					mpz_and(value, value, sec);

				// evaluate as a binary or
				else if(oper.get_text() == lexer::BINARY_OPER_DATA[lexer::OR])
					mpz_ior(value, value, sec);

				// evaluate as a binary xor
				else if(oper.get_text() == lexer::BINARY_OPER_DATA[lexer::XOR])
					mpz_xor(value, value, sec);

				else
					throw exc_code::INVALID_BINARY_OPERATOR;
				token::convert_to_string(value, output);
				mpz_clear(value);
				mpz_clear(sec);
			} else
				throw exc_code::EXPECTING_INTEGER_OPERAND;
			break;

		// evaluate as a logical operator
		case token::LOGICAL_OPER:

			// check for correct type
			if(type == token::INTEGER) {
				mpz_t value;
				uint64_t sec;
				token::convert_to_integer(value, accum.get_text());
				sec = token::convert_to<uint64_t>(second.get_text());

				// evaluate as a logical left shift
				if(oper.get_text() == lexer::LOGICAL_OPER_DATA[lexer::LEFT_SHIFT])
					mpz_mul_2exp(value, value, sec);

				// evaluate as a logical right shift
				else if(oper.get_text() == lexer::LOGICAL_OPER_DATA[lexer::RIGHT_SHIFT]) {
					mpz_tdiv_q_2exp(value, value, sec);

				} else
					throw exc_code::INVALID_LOGICAL_OPERATOR;
				token::convert_to_string(value, output);
				mpz_clear(value);
			} else
				throw exc_code::EXPECTING_INTEGER_OPERAND;
			break;

		// evaluate as an arithmetic operator
		case token::OPER:

			// evaluate integer values
			if(type == token::INTEGER) {
				mpz_t value, sec;
				token::convert_to_integer(value, accum.get_text());
				token::convert_to_integer(sec, second.get_text());

				// evaluate as a arithmetic plus
				if(oper.get_text() == lexer::OPER_DATA[lexer::PLUS])
					mpz_add(value, value, sec);

				// evaluate as a arithmetic minus
				else if(oper.get_text() == lexer::OPER_DATA[lexer::MINUS])
					mpz_sub(value, value, sec);

				// evaluate as a arithmetic multiply
				else if(oper.get_text() == lexer::OPER_DATA[lexer::MULTI])
					mpz_mul(value, value, sec);

				// evaluate as a arithmetic divide
				else if(oper.get_text() == lexer::OPER_DATA[lexer::DIV])
					mpz_div(value, value, sec);

				// evaluate as a arithmetic modulo
				else if(oper.get_text() == lexer::OPER_DATA[lexer::MOD])
					mpz_mod(value, value, sec);

				// evaluate as a arithmetic power
				else if(oper.get_text() == lexer::OPER_DATA[lexer::POW]) {
					uint64_t exp;
					token::convert_to_integer(value, accum.get_text());
					exp = token::convert_to<uint64_t>(second.get_text());
					mpz_pow_ui(value, value, exp);
				}
				token::convert_to_string(value, output);
				mpz_clear(value);
				mpz_clear(sec);

			// evauate floating-point values
			} else if(type == token::FLOAT) {
				mpf_t value, sec;
				token::convert_to_float(value, accum.get_text());
				token::convert_to_float(sec, second.get_text());

				// evaluate as a arithmetic plus
				if(oper.get_text() == lexer::OPER_DATA[lexer::PLUS])
					mpf_add(value, value, sec);

				// evaluate as a arithmetic minus
				else if(oper.get_text() == lexer::OPER_DATA[lexer::MINUS])
					mpf_sub(value, value, sec);

				// evaluate as a arithmetic multiply
				else if(oper.get_text() == lexer::OPER_DATA[lexer::MULTI])
					mpf_mul(value, value, sec);

				// evaluate as a arithmetic divide
				else if(oper.get_text() == lexer::OPER_DATA[lexer::DIV])
					mpf_div(value, value, sec);

				// evaluate as a arithmetic modulo
				else if(oper.get_text() == lexer::OPER_DATA[lexer::MOD])
					throw exc_code::EXPECTING_INTEGER_OPERAND;

				// evaluate as a arithmetic power
				else if(oper.get_text() == lexer::OPER_DATA[lexer::POW]) {
					uint64_t exp;
					token::convert_to_float(value, accum.get_text());
					exp = token::convert_to<uint64_t>(second.get_text());
					mpf_pow_ui(value, value, exp);
				}
				token::convert_to_string(value, output);
				mpf_clear(value);
				mpf_clear(sec);
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
