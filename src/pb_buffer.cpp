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

#include <sstream>
#include "exc_code.hpp"
#include "pb_buffer.hpp"

/*
 * Pushback Buffer constructor
 */
pb_buffer::pb_buffer(void) {
	ch = 0;
	pos = 0;
}

/*
 * Pushback Buffer constructor
 */
pb_buffer::pb_buffer(const pb_buffer &other) {
	ch = 0;
	pos = 0;
	input.assign(other.input);
	stream.clear();
}

/*
 * Pushback Buffer constructor
 */
pb_buffer::pb_buffer(const std::string &input) {
	ch = 0;
	pos = 0;
	this->input.assign(input);
}

/*
 * Pushback Buffer destructor
 */
pb_buffer::~pb_buffer(void) {
	input.clear();
	stream.clear();
}

/*
 * Pushback Buffer assignment
 */
pb_buffer &pb_buffer::operator=(const pb_buffer &other) {

	// check for self-assignment
	if(this == &other)
		return *this;

	// set attributes
	ch = 0;
	pos = 0;
	input.assign(other.input);
	stream.clear();
	return *this;
}

/*
 * Pushback Buffer equivalence
 */
bool pb_buffer::operator==(const pb_buffer &other) {

	// check for same object
	if(this == &other)
		return true;

	// check that all attributes are the same
	return ch == other.ch
			&& input == other.input
			&& pos == other.pos;
}

/*
 * Read the current character and advance the buffer one character forward
 */
bool pb_buffer::operator>>(char &ch) {

	// check if stream has next & advance stream
	if(!next())
		return false;

	// set ch to current character in stream
	ch = get_current();
	return true;
}

/*
 * Read the current character and regress the buffer one character backwards
 */
bool pb_buffer::operator<<(char &ch) {

	// check if stream has previous & regress stream
	if(!previous())
		return false;

	// set ch to current character in stream
	ch = get_current();
	return true;
}

/*
 * Initialize buffer
 */
void pb_buffer::initialize(void) {
	
	// initialize stream
	stream >> std::noskipws;
	stream << input;
	pos = 0;
	next();
}

/*
 * Advances the buffer by one character
 */
bool pb_buffer::next(void) {

	// check if stream has next
	if(!has_next())
		return false;

	// seek to the next character in stream
	stream >> ch;
	pos++;
	return true;
}

/*
 * Regresses the buffer by one character
 */
bool pb_buffer::previous(void) {

	// check if stream has previous
	if(!stream.tellg())
		return false;

	// seek two characters backwards
	stream.seekg(-2, std::ios_base::cur);

	// set ch to current character in stream
	stream >> ch;
	pos--;
	return true;
}
