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

#ifndef PB_BUFFER_HPP_
#define PB_BUFFER_HPP_

#include <fstream>
#include <sstream>
#include <string>

class pb_buffer {
private:

	char ch;
	unsigned int pos;
	std::string input;
	std::stringstream stream;

public:

	/*
	 * Pushback Buffer constructor
	 */
	pb_buffer(void);

	/*
	 * Pushback Buffer constructor
	 */
	pb_buffer(const pb_buffer &other);

	/*
	 * Pushback Buffer constructor
	 */
	pb_buffer(const std::string &input);

	/*
	 * Pushback Buffer destructor
	 */
	virtual ~pb_buffer(void);

	/*
	 * Pushback Buffer assignment
	 */
	pb_buffer &operator=(const pb_buffer &other);

	/*
	 * Pushback Buffer equivalence
	 */
	bool operator==(const pb_buffer &other);

	/*
	 * Pushback Buffer equivalence
	 */
	bool operator!=(const pb_buffer &other) { return !(*this == other); }

	/*
	 * Read the current character and advance the buffer one character forward
	 */
	bool operator>>(char &ch);

	/*
	 * Read the current character and regress the buffer one character backwards
	 */
	bool operator<<(char &ch);

	/*
	 * Returns the most recently read character
	 */
	char get_current(void) { return ch; }

	/*
	 * Returns the buffer's given input
	 */
	std::string &get_input(void) { return input; }

	/*
	 * Returns the buffer's current position in the stream
	 */
	unsigned int get_position(void) { return pos; }

	/*
	 * Returns the status of the buffer
	 */
	bool has_next(void) { return stream.good(); }

	/*
	 * Initialize buffer
	 */
	void initialize(void);

	/*
	 * Advances the buffer by one character
	 */
	bool next(void);

	/*
	 * Regresses the buffer by one character
	 */
	bool previous(void);

	/*
	 * Resets the buffer to an initial state
	 */
	void reset(void) { stream.clear(); }

	/*
	 * Returns a string representation of the current state of the buffer
	 */
	void to_string(std::string &str) { str += get_current(); }
};

#endif
