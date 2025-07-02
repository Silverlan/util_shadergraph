// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <unordered_map>
#include <udm.hpp>
#include <udm_enums.hpp>
#include <sstream>
#include <cassert>
#include <typeinfo>

export module pragma.shadergraph:socket;

import :parameter;

#pragma optimize("", off)
export namespace pragma::shadergraph {
	struct Socket : Parameter {
		enum class Flags : uint32_t {
			None = 0,
			Linkable = 1u,
		};
		Flags flags = Flags::None;

		Socket(const std::string &name, DataType type);
		Socket(const Socket &other);
		bool IsLinkable() const { return umath::is_flag_set(flags, Flags::Linkable); }
		~Socket();
	};
};
export {REGISTER_BASIC_BITWISE_OPERATORS(pragma::shadergraph::Socket::Flags)};
