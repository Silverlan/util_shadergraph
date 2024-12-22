/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string>
#include <string_view>
#include <stdexcept>
#include <cassert>
#include <memory>

module pragma.shadergraph;

import :socket;

using namespace pragma::shadergraph;

Socket::Socket(const std::string &name, DataType type) : Parameter {name, type} {}

Socket::Socket(const Socket &other) : Parameter {other} { flags = other.flags; }

Socket::~Socket() {}
