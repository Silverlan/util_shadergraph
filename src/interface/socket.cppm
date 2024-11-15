/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <udm.hpp>
#include <udm_enums.hpp>
#include <sstream>
#include <cassert>

export module pragma.shadergraph:socket;
#pragma optimize("", off)
export namespace pragma::shadergraph {
	enum class SocketType : uint8_t {
		Boolean = 0,
		Int,
		UInt,
		UInt64,
		Float,
		Color,
		Vector,
		Point,
		Normal,
		Point2,
		String,
		Transform,
		Enum,
	};

	constexpr udm::Type to_udm_type(SocketType type)
	{
		switch(type) {
		case SocketType::Boolean:
			return udm::Type::Boolean;
		case SocketType::Int:
		case SocketType::Enum:
			return udm::Type::Int32;
		case SocketType::UInt:
			return udm::Type::UInt32;
		case SocketType::UInt64:
			return udm::Type::UInt64;
		case SocketType::Float:
			return udm::Type::Float;
		case SocketType::Color:
		case SocketType::Vector:
		case SocketType::Point:
		case SocketType::Normal:
			return udm::Type::Vector3;
		case SocketType::Point2:
			return udm::Type::Vector2;
		case SocketType::String:
			return udm::Type::String;
		case SocketType::Transform:
			return udm::Type::Mat3x4;
		}
		return udm::Type::Invalid;
	}

	constexpr std::variant<udm::tag_t<udm::Boolean>, udm::tag_t<udm::Int32>, udm::tag_t<udm::UInt32>, udm::tag_t<udm::UInt64>, udm::tag_t<udm::Float>, udm::tag_t<udm::Vector3>, udm::tag_t<udm::Vector2>, udm::tag_t<udm::String>, udm::tag_t<udm::Transform>, udm::tag_t<udm::Mat3x4>> get_tag(
	  SocketType e)
	{
		switch(e) {
		case SocketType::Boolean:
			return udm::tag<udm::Boolean>;
		case SocketType::Int:
		case SocketType::Enum:
			return udm::tag<udm::Int32>;
		case SocketType::UInt:
			return udm::tag<udm::UInt32>;
		case SocketType::UInt64:
			return udm::tag<udm::UInt64>;
		case SocketType::Float:
			return udm::tag<udm::Float>;
		case SocketType::Color:
		case SocketType::Vector:
		case SocketType::Point:
		case SocketType::Normal:
			return udm::tag<udm::Vector3>;
		case SocketType::Point2:
			return udm::tag<udm::Vector2>;
		case SocketType::String:
			return udm::tag<udm::String>;
		case SocketType::Transform:
			return udm::tag<udm::Mat3x4>;
		}
		return {};
	}

	template<typename T>
	constexpr bool is_socket_type()
	{
		return std::is_same_v<T, udm::Boolean> || std::is_same_v<T, udm::Int32> || std::is_same_v<T, udm::UInt32> || std::is_same_v<T, udm::UInt64> || std::is_same_v<T, udm::Float> || std::is_same_v<T, udm::Vector3> || std::is_same_v<T, udm::Vector2> || std::is_same_v<T, udm::String>
		  || std::is_same_v<T, udm::Mat3x4>;
	}

	template<typename T>
	concept is_socket_type_v = is_socket_type<T>();

	template<typename T>
	constexpr decltype(auto) visit(SocketType type, T vs)
	{
		return std::visit(vs, get_tag(type));
	}

	struct SocketValue {
	  public:
		SocketValue(const SocketValue &other);
		SocketValue(SocketType type);
		~SocketValue();

		SocketValue &operator=(const SocketValue &other);

		template<typename T>
		bool Set(const T &val)
		{
			if constexpr(std::is_enum_v<T>)
				return Set(umath::to_integral(val));
			return visit(m_type, [this, &val](auto tag) {
				using TFrom = decltype(val);
				using TTo = typename decltype(tag)::type;
				if constexpr(udm::is_convertible<TFrom, TTo>()) {
					if(!m_value)
						m_value = new TTo {};
					*static_cast<TTo *>(m_value) = udm::convert<TFrom, TTo>(val);
					return true;
				}
				return false;
			});
		}

		template<typename T>
		bool Get(T &outVal) const
		{
			if constexpr(std::is_enum_v<T>)
				return Get(reinterpret_cast<std::underlying_type_t<T> &>(outVal));
			return visit(m_type, [this, &outVal](auto tag) {
				using TFrom = typename decltype(tag)::type;
				using TTo = std::remove_cv_t<std::remove_reference_t<decltype(outVal)>>;
				if(!m_value)
					return false;
				if constexpr(udm::is_convertible<TFrom, TTo>()) {
					outVal = udm::convert<TFrom, TTo>(*static_cast<TFrom *>(m_value));
					return true;
				}
				return false;
			});
		}

		void Clear();
		operator bool() const;
	  private:
		SocketType m_type;
		udm::DataValue m_value = nullptr;
	};

	struct Socket {
		std::string name;
		SocketType type;
		SocketValue defaultValue;

		Socket(const std::string &name, SocketType type);
	};
};
