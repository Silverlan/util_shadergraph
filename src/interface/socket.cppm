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
#include <memory>
#include <unordered_map>
#include <udm.hpp>
#include <udm_enums.hpp>
#include <sstream>
#include <cassert>
#include <typeinfo>

export module pragma.shadergraph:socket;

import :enum_set;

#pragma optimize("", off)
export namespace pragma::shadergraph {
	enum class SocketType : uint8_t {
		Boolean = 0,
		Int,
		UInt,
		Float,
		Color,
		Vector,
		Point,
		Normal,
		Point2,
		String,
		Transform,
		Enum,

		Invalid = std::numeric_limits<uint8_t>::max(),
	};

	constexpr SocketType to_socket_type(udm::Type type)
	{
		switch(type) {
		case udm::Type::Boolean:
			return SocketType::Boolean;
		case udm::Type::Int32:
			return SocketType ::Int;
		case udm::Type::UInt32:
			return SocketType ::UInt;
		case udm::Type::Float:
			return SocketType ::Float;
		case udm::Type::Vector3:
			return SocketType::Vector;
		case udm::Type::Vector2:
			return SocketType::Point2;
		case udm::Type::String:
			return SocketType ::String;
		case udm::Type::Mat4:
			return SocketType::Transform;
		}
		return SocketType::Invalid;
	}

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
			return udm::Type::Mat4;
		}
		return udm::Type::Invalid;
	}

	constexpr const char *to_glsl_type(SocketType type)
	{
		switch(type) {
		case SocketType::Boolean:
			return "bool";
		case SocketType::Int:
		case SocketType::Enum:
			return "int";
		case SocketType::UInt:
			return "uint";
		case SocketType::Float:
			return "float";
		case SocketType::Color:
			return "vec3";
		case SocketType::Vector:
		case SocketType::Point:
		case SocketType::Normal:
			return "vec3";
		case SocketType::Point2:
			return "vec2";
		case SocketType::Transform:
			return "mat4";
		case SocketType::String:
			return nullptr;
		}
		return nullptr;
	}

	constexpr std::variant<udm::tag_t<udm::Boolean>, udm::tag_t<udm::Int32>, udm::tag_t<udm::UInt32>, udm::tag_t<udm::UInt64>, udm::tag_t<udm::Float>, udm::tag_t<udm::Vector3>, udm::tag_t<udm::Vector2>, udm::tag_t<udm::String>, udm::tag_t<udm::Mat4>> get_tag(SocketType e)
	{
		switch(e) {
		case SocketType::Boolean:
			return udm::tag<udm::Boolean>;
		case SocketType::Int:
		case SocketType::Enum:
			return udm::tag<udm::Int32>;
		case SocketType::UInt:
			return udm::tag<udm::UInt32>;
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
			return udm::tag<udm::Mat4>;
		}
		return {};
	}

	template<typename T>
	constexpr bool is_socket_type()
	{
		return std::is_same_v<T, udm::Boolean> || std::is_same_v<T, udm::Int32> || std::is_same_v<T, udm::UInt32> || std::is_same_v<T, udm::UInt64> || std::is_same_v<T, udm::Float> || std::is_same_v<T, udm::Vector3> || std::is_same_v<T, udm::Vector2> || std::is_same_v<T, udm::String>
		  || std::is_same_v<T, udm::Mat4>;
	}

	template<typename T>
	concept is_socket_type_v = is_socket_type<T>();

	template<typename T>
	constexpr decltype(auto) visit(SocketType type, T vs)
	{
		return std::visit(vs, get_tag(type));
	}

	template<typename T>
	    requires(is_socket_type<T>() && !std::is_same_v<T, udm::String>)
	std::string to_glsl_value(const T &value)
	{
		if constexpr(std::is_same_v<T, udm::Boolean>)
			return value ? "1.0" : "0.0";
		else if constexpr(std::is_same_v<T, udm::Int32> || std::is_same_v<T, udm::UInt32> || std::is_same_v<T, udm::UInt64> || std::is_same_v<T, udm::Float>)
			return std::to_string(value);
		else if constexpr(std::is_same_v<T, udm::Vector3>)
			return "vec3(" + std::to_string(value.x) + ", " + std::to_string(value.y) + ", " + std::to_string(value.z) + ")";
		else if constexpr(std::is_same_v<T, udm::Vector2>)
			return "vec2(" + std::to_string(value.x) + ", " + std::to_string(value.y) + ")";
		throw std::invalid_argument {"Socket type '" + std::string {typeid(T).name()} + "' cannot be converted to GLSL!"};
		return {};
	}

	struct SocketValue {
	  public:
		SocketValue(const SocketValue &other);
		SocketValue(SocketValue &&other);
		SocketValue(SocketType type);
		~SocketValue();

		SocketValue &operator=(const SocketValue &other);
		SocketValue &operator=(SocketValue &&other);

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
		SocketType GetType() const { return m_type; }
		const udm::DataValue GetData() const { return m_value; }
		operator bool() const;
	  private:
		SocketType m_type;
		udm::DataValue m_value = nullptr;
	};

	struct Socket {
		std::string name;
		SocketType type;
		SocketValue defaultValue;
		std::unique_ptr<EnumSet> enumSet;

		Socket(const std::string &name, SocketType type);
		Socket(const Socket &other);
		~Socket();
	};
};
