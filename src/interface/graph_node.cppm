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

export module pragma.shadergraph:graph_node;

import :socket;
import :node;
#pragma optimize("", off)
export namespace pragma::shadergraph {
	struct NodeInstance;
	struct InputTest;
	struct OutputTest {
		OutputTest(NodeInstance &node, uint32_t index);
		const Socket &GetSocket() const;
		NodeInstance *parent = nullptr;
		std::vector<InputTest *> links;
		uint32_t outputIndex = std::numeric_limits<uint32_t>::max();
	};

	struct InputTest {
		InputTest(NodeInstance &node, uint32_t index);
		~InputTest();
		NodeInstance *parent = nullptr;
		OutputTest *link = nullptr;
		uint32_t inputIndex = std::numeric_limits<uint32_t>::max();

		const Socket &GetSocket() const;
		template<typename T>
		bool SetValue(const T &val);
		template<typename T>
		bool GetValue(T &outVal) const;
		void ClearValue();
	  private:
		SocketValue value;
	};

	struct NodeInstance {
		const Node &node;
		std::vector<InputTest> inputs;
		std::vector<OutputTest> outputs;
		NodeInstance(Node &node, const std::string &name);
		std::string GetName() const;
		template<typename T>
		bool SetInputValue(const char *inputName, const T &val)
		{
			auto inputIdx = node.FindInputIndex(inputName);
			if(!inputIdx)
				return false;
			auto &input = inputs[*inputIdx];
			return input.SetValue<T>(val);
		}
		template<typename T>
		bool GetInputValue(const char *inputName, T &outVal)
		{
			auto inputIdx = node.FindInputIndex(inputName);
			if(!inputIdx)
				return false;
			auto &input = inputs[*inputIdx];
			return input.GetValue<T>(outVal);
		}
		void ClearInputValue(const char *inputName);
		bool Disconnect(const char *inputName);
		bool Disconnect(const char *outputName, NodeInstance &linkTarget, const char *inputName);
		bool CanLink(const char *outputName, NodeInstance &linkTarget, const char *inputName) const;
		bool Link(const char *outputName, NodeInstance &linkTarget, const char *inputName);

		std::string GetOutputVarName(size_t outputIdx) const;
		std::string GetOutputVarName(const std::string_view &name) const;

		uint32_t nodeIndex = std::numeric_limits<uint32_t>::max();
		std::string m_name;
	};

	template<typename T>
	bool InputTest::SetValue(const T &val)
	{
		return value.Set<T>(val);
	}
	template<typename T>
	bool InputTest::GetValue(T &outVal) const
	{
		if(value)
			return value.Get<T>(outVal);
		return GetSocket().defaultValue.Get<T>(outVal);
	}
};
