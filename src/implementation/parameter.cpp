// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <cassert>

module pragma.shadergraph;

import :parameter;

using namespace pragma::shadergraph;

Value::Value(const Value &other) : Value {other.m_type} { operator=(other); }
Value::Value(Value &&other) : Value {other.m_type} { operator=(std::move(other)); }
Value::Value(DataType type) : m_type {type} {}
Value::~Value() { Clear(); }

Value &Value::operator=(const Value &other)
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
Value &Value::operator=(Value &&other)
{
	Clear();
	m_type = other.m_type;
	if(!other)
		return *this;
	m_value = other.m_value;
	other.m_value = nullptr;
	return *this;
}

void Value::Clear()
{
	if(!m_value)
		return;
	visit(m_type, [this](auto tag) {
		using T = typename decltype(tag)::type;
		delete static_cast<T *>(m_value);
	});
	m_value = nullptr;
}

Value::operator bool() const { return m_value != nullptr; }

Parameter::Parameter(const std::string &name, DataType type) : name(name), type(type), defaultValue {type} {}

Parameter::Parameter(const Parameter &other) : Parameter {other.name, other.type}
{
	defaultValue = other.defaultValue;
	min = other.min;
	max = other.max;
	enumSet = other.enumSet ? std::make_unique<EnumSet>(*other.enumSet) : nullptr;
}

Parameter &Parameter::operator=(const Parameter &other)
{
	name = other.name;
	type = other.type;
	defaultValue = other.defaultValue;
	min = other.min;
	max = other.max;
	enumSet = other.enumSet ? std::make_unique<EnumSet>(*other.enumSet) : nullptr;
	return *this;
}
