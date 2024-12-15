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

import :node_registry;

using namespace pragma::shadergraph;

SocketValue::SocketValue(const SocketValue &other) : SocketValue {other.m_type} { operator=(other); }
SocketValue::SocketValue(SocketValue &&other) : SocketValue {other.m_type} { operator=(std::move(other)); }
SocketValue::SocketValue(SocketType type) : m_type {type} {}
SocketValue::~SocketValue() { Clear(); }

SocketValue &SocketValue::operator=(const SocketValue &other)
{
	Clear();
	m_type = other.m_type;
	if(!other)
		return *this;
	visit(m_type, [this, &other](auto tag) {
		using T = typename decltype(tag)::type;
		T val;
		auto res = other.Get<T>(val);
		assert(res);
		Set<T>(val);
	});
	return *this;
}
SocketValue &SocketValue::operator=(SocketValue &&other)
{
	Clear();
	m_type = other.m_type;
	if(!other)
		return *this;
	m_value = other.m_value;
	other.m_value = nullptr;
	return *this;
}

void SocketValue::Clear()
{
	if(!m_value)
		return;
	visit(m_type, [this](auto tag) {
		using T = typename decltype(tag)::type;
		delete static_cast<T *>(m_value);
	});
	m_value = nullptr;
}

SocketValue::operator bool() const { return m_value != nullptr; }

Socket::Socket(const std::string &name, SocketType type) : name(name), type(type), defaultValue {type} {}

Socket::Socket(const Socket &other) : Socket {other.name, other.type}
{
	defaultValue = other.defaultValue;
	enumSet = other.enumSet ? std::make_unique<EnumSet>(*other.enumSet) : nullptr;
}

Socket::~Socket() {}
