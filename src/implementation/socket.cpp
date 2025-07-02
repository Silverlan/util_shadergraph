// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

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
