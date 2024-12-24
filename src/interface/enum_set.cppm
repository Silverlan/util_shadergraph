/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string_view>
#include <string>
#include <memory>
#include <sharedutils/util_case_insensitive_hash.hpp>
#include <sharedutils/magic_enum.hpp>

export module pragma.shadergraph:enum_set;

export namespace pragma::shadergraph {
	struct EnumSet {
	  public:
		template<typename T>
		static std::unique_ptr<EnumSet> Create()
		{
			auto &names = magic_enum::enum_names<T>();
			auto enumSet = std::make_unique<EnumSet>();
			enumSet->m_nameToValue.reserve(names.size());
			enumSet->m_valueToName.reserve(names.size());
			for(size_t i = 0; i < names.size(); ++i) {
				if(names[i] == "Count" || names[i] == "Invalid")
					continue;
				enumSet->insert(std::string {names[i]}, i);
			}
			return enumSet;
		}
		EnumSet() = default;
		bool empty() const { return m_nameToValue.empty(); }
		void insert(const std::string &name, int32_t value)
		{
			m_nameToValue[name] = value;
			m_valueToName[value] = name;
		}
		void reserve(size_t size)
		{
			m_nameToValue.reserve(size);
			m_valueToName.reserve(size);
		}
		bool exists(const std::string &name) const { return m_nameToValue.find(name) != m_nameToValue.end(); }
		bool exists(int32_t value) const { return m_valueToName.find(value) != m_valueToName.end(); }
		std::optional<int32_t> findValue(const std::string &name) const
		{
			auto it = m_nameToValue.find(name);
			if(it == m_nameToValue.end())
				return std::nullopt;
			return it->second;
		}
		std::optional<std::string> findName(int32_t value) const
		{
			auto it = m_valueToName.find(value);
			if(it == m_valueToName.end())
				return std::nullopt;
			return it->second;
		}
		const util::CIMap<int32_t> &getNameToValue() const { return m_nameToValue; }
		const std::unordered_map<int32_t, std::string> &getValueToName() const { return m_valueToName; }
	  private:
		util::CIMap<int32_t> m_nameToValue;
		std::unordered_map<int32_t, std::string> m_valueToName;
	};
};
