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

#include "exc_code.hpp"

/*
 * Exception message
 */
const std::string exc_code::MESSAGE[22] = {

	/*
	 * General exceptions
	 */
	"Success",
	"Failed to allocate memory",
	"Exit",
	"EOF",

	/*
	 * Parser exceptions
	 */
	"Expecting identifier",
	"Expecting close parenthesis",
	"Expecting close bracket",

	/*
	 * Runtime exceptions
	 */
	"Invalid operator",
	"Invalid operand",
	"Expecting integer operands",
	"Invalid binary operator",
	"Invalid logical operator",
	"Invalid arithmetic operator",
	"Invalid expression",
	"Invalid constant",
	"Invalid function",
	"Invalid statement",
	"Invalid assignment statement",
	"Expecting alpha-numeric identifier",
	"Undefined identifier",
	"Expecting positive integer operands",
	"Invalid unary operator",
};
