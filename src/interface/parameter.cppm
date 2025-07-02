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

export module pragma.shadergraph:parameter;

import :enum_set;

export namespace pragma::shadergraph {
	enum class DataType : uint8_t {
		Boolean = 0,
		Int,
		UInt,
		Float,
		Half,
		UInt16,
		Color,
		Vector,
		Vector4,
		Point,
		Normal,
		Point2,
		String,
		Transform,
		Enum,
		Count,

		Invalid = std::numeric_limits<uint8_t>::max(),
	};

	constexpr DataType to_data_type(udm::Type type)
	{
		switch(type) {
		case udm::Type::Boolean:
			return DataType::Boolean;
		case udm::Type::Int32:
			return DataType ::Int;
		case udm::Type::UInt32:
			return DataType ::UInt;
		case udm::Type::Float:
			return DataType ::Float;
		case udm::Type::Half:
			return DataType ::Half;
		case udm::Type::UInt16:
			return DataType ::UInt16;
		case udm::Type::Vector3:
			return DataType::Vector;
		case udm::Type::Vector4:
			return DataType::Vector4;
		case udm::Type::Vector2:
			return DataType::Point2;
		case udm::Type::String:
			return DataType ::String;
		case udm::Type::Mat4:
			return DataType::Transform;
		}
		static_assert(umath::to_integral(DataType::Count) == 15, "Update the list above when adding new types!");
		return DataType::Invalid;
	}

	constexpr udm::Type to_udm_type(DataType type)
	{
		switch(type) {
		case DataType::Boolean:
			return udm::Type::Boolean;
		case DataType::Int:
		case DataType::Enum:
			return udm::Type::Int32;
		case DataType::UInt:
			return udm::Type::UInt32;
		case DataType::Float:
			return udm::Type::Float;
		case DataType::Half:
			return udm::Type::Half;
		case DataType::UInt16:
			return udm::Type::UInt16;
		case DataType::Color:
		case DataType::Vector:
		case DataType::Point:
		case DataType::Normal:
			return udm::Type::Vector3;
		case DataType::Vector4:
			return udm::Type::Vector4;
		case DataType::Point2:
			return udm::Type::Vector2;
		case DataType::String:
			return udm::Type::String;
		case DataType::Transform:
			return udm::Type::Mat4;
		}
		static_assert(umath::to_integral(DataType::Count) == 15, "Update the list above when adding new types!");
		return udm::Type::Invalid;
	}

	constexpr const char *to_glsl_type(DataType type)
	{
		switch(type) {
		case DataType::Boolean:
			return "bool";
		case DataType::Int:
		case DataType::Enum:
			return "int";
		case DataType::UInt:
			return "uint";
		case DataType::Float:
		case DataType::Half:
			return "float";
		case DataType::UInt16:
			return "uint";
		case DataType::Color:
			return "vec3";
		case DataType::Vector:
		case DataType::Point:
		case DataType::Normal:
			return "vec3";
		case DataType::Vector4:
			return "vec4";
		case DataType::Point2:
			return "vec2";
		case DataType::Transform:
			return "mat4";
		case DataType::String:
			return nullptr;
		}
		static_assert(umath::to_integral(DataType::Count) == 15, "Update the list above when adding new types!");
		return nullptr;
	}

	constexpr std::variant<udm::tag_t<udm::Boolean>, udm::tag_t<udm::Int32>, udm::tag_t<udm::UInt32>, udm::tag_t<udm::UInt64>, udm::tag_t<udm::Float>, udm::tag_t<udm::UInt16>, udm::tag_t<udm::Half>, udm::tag_t<udm::Vector3>, udm::tag_t<udm::Vector4>, udm::tag_t<udm::Vector2>,
	  udm::tag_t<udm::String>, udm::tag_t<udm::Mat4>>
	get_tag(DataType e)
	{
		switch(e) {
		case DataType::Boolean:
			return udm::tag<udm::Boolean>;
		case DataType::Int:
		case DataType::Enum:
			return udm::tag<udm::Int32>;
		case DataType::UInt:
			return udm::tag<udm::UInt32>;
		case DataType::Float:
			return udm::tag<udm::Float>;
		case DataType::UInt16:
			return udm::tag<udm::UInt16>;
		case DataType::Half:
			return udm::tag<udm::Half>;
		case DataType::Color:
		case DataType::Vector:
		case DataType::Point:
		case DataType::Normal:
			return udm::tag<udm::Vector3>;
		case DataType::Vector4:
			return udm::tag<udm::Vector4>;
		case DataType::Point2:
			return udm::tag<udm::Vector2>;
		case DataType::String:
			return udm::tag<udm::String>;
		case DataType::Transform:
			return udm::tag<udm::Mat4>;
		}
		static_assert(umath::to_integral(DataType::Count) == 15, "Update the list above when adding new types!");
		return {};
	}

	template<typename T>
	constexpr bool is_data_type()
	{
		static_assert(umath::to_integral(DataType::Count) == 15, "Update this list when adding new types!");
		return std::is_same_v<T, udm::Boolean> || std::is_same_v<T, udm::Int32> || std::is_same_v<T, udm::UInt32> || std::is_same_v<T, udm::UInt64> || std::is_same_v<T, udm::Float> || std::is_same_v<T, udm::UInt16> || std::is_same_v<T, udm::Half> || std::is_same_v<T, udm::Vector3>
		  || std::is_same_v<T, udm::Vector4> || std::is_same_v<T, udm::Vector2> || std::is_same_v<T, udm::String> || std::is_same_v<T, udm::Mat4>;
	}

	constexpr bool is_data_type_compatible(DataType a, DataType b)
	{
		if(a == b)
			return true;
		static_assert(umath::to_integral(DataType::Count) == 15, "Update this list when adding new types!");
		switch(a) {
		case DataType::Boolean:
		case DataType::Int:
		case DataType::UInt:
		case DataType::Float:
		case DataType::UInt16:
		case DataType::Half:
		case DataType::Enum:
			{
				switch(b) {
				case DataType::Boolean:
				case DataType::Int:
				case DataType::UInt:
				case DataType::Float:
				case DataType::UInt16:
				case DataType::Half:
				case DataType::Enum:
					return true;
				}
				return false;
			}
		case DataType::Color:
		case DataType::Vector:
		case DataType::Point:
		case DataType::Normal:
			{
				switch(b) {
				case DataType::Color:
				case DataType::Vector:
				case DataType::Point:
				case DataType::Normal:
				case DataType::Vector4:
					return true;
				}
				return false;
			}
		}
		return false;
	}

	template<typename T>
	concept is_data_type_v = is_data_type<T>();

	template<typename T>
	constexpr decltype(auto) visit(DataType type, T vs)
	{
		return std::visit(vs, get_tag(type));
	}

	template<typename T>
	    requires(is_data_type<T>() && !std::is_same_v<T, udm::String>)
	std::string to_glsl_value(const T &value)
	{
		if constexpr(std::is_same_v<T, udm::Boolean>)
			return value ? "1.0" : "0.0";
		else if constexpr(std::is_same_v<T, udm::Int32> || std::is_same_v<T, udm::UInt32> || std::is_same_v<T, udm::UInt64> || std::is_same_v<T, udm::Float>)
			return std::to_string(value);
		else if constexpr(std::is_same_v<T, udm::Half>)
			return std::to_string(static_cast<float>(value));
		else if constexpr(std::is_same_v<T, udm::Vector3>)
			return "vec3(" + std::to_string(value.x) + ", " + std::to_string(value.y) + ", " + std::to_string(value.z) + ")";
		else if constexpr(std::is_same_v<T, udm::Vector2>)
			return "vec2(" + std::to_string(value.x) + ", " + std::to_string(value.y) + ")";
		throw std::invalid_argument {"Socket type '" + std::string {typeid(T).name()} + "' cannot be converted to GLSL!"};
		return {};
	}

	struct Value {
	  public:
		template<typename T>
		    requires(is_data_type_v<T>)
		static Value Create(const T &val)
		{
			Value v {to_data_type(udm::type_to_enum<T>())};
			v.Set(val);
			return v;
		}
		Value(const Value &other);
		Value(Value &&other);
		Value(DataType type);
		~Value();

		Value &operator=(const Value &other);
		Value &operator=(Value &&other);
		template<typename T>
		Value &operator=(const T &val)
		{
			Set(val);
			return *this;
		}

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
		DataType GetType() const { return m_type; }
		const udm::DataValue GetData() const { return m_value; }
		operator bool() const;
	  private:
		DataType m_type;
		udm::DataValue m_value = nullptr;
	};

	struct Parameter {
		Parameter(const std::string &name, DataType type);
		Parameter(const Parameter &other);
		Parameter &operator=(const Parameter &other);

		void SetRange(float min, float max)
		{
			this->min = min;
			this->max = max;
		}
		void SetEnumValues(const std::unordered_map<std::string, uint32_t> &values)
		{
			enumSet = std::make_unique<EnumSet>();
			enumSet->reserve(values.size());
			for(auto &pair : values)
				enumSet->insert(pair.first, pair.second);
		}

		std::string name;
		DataType type;
		Value defaultValue;
		std::unique_ptr<EnumSet> enumSet;

		float min = 0.0f;
		float max = 1.0f;
	};
};
