// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include "util_enum_flags.hpp"

#include <cassert>

export module pragma.shadergraph:socket;

import :parameter;

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
	using namespace umath::scoped_enum::bitwise;
};
export {
	REGISTER_ENUM_FLAGS(pragma::shadergraph::Socket::Flags)
}
