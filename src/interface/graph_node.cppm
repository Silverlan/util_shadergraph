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
	struct GraphNode;
	struct InputTest;
	struct OutputTest {
		OutputTest(GraphNode &node, uint32_t index);
		const Socket &GetSocket() const;
		GraphNode *parent = nullptr;
		std::vector<InputTest *> links;
		uint32_t outputIndex = std::numeric_limits<uint32_t>::max();
	};

	struct InputTest {
		InputTest(GraphNode &node, uint32_t index);
		~InputTest();
		GraphNode *parent = nullptr;
		OutputTest *link = nullptr;
		uint32_t inputIndex = std::numeric_limits<uint32_t>::max();

		const Socket &GetSocket() const;
		template<typename T>
		bool SetValue(const T &val);
		template<typename T>
		bool GetValue(T &outVal) const;
		void ClearValue();
		bool HasValue() const;
	  private:
		SocketValue value;
	};

	class Graph;
	struct GraphNode {
		struct SocketLink {
			std::string outputNode;
			std::string outputName;

			InputTest *inputSocket;
		};

		const Node &node;
		std::vector<InputTest> inputs;
		std::vector<OutputTest> outputs;
		GraphNode(Graph &graph, Node &node, const std::string &name);
		const Node *operator->() const { return &node; }
		std::string GetName() const;
		void SetDisplayName(const std::optional<std::string> &displayName) { m_displayName = displayName; }
		const std::optional<std::string> &GetDisplayName() const { return m_displayName; }
		template<typename T>
		bool SetInputValue(const std::string_view &inputName, const T &val)
		{
			auto inputIdx = node.FindInputIndex(inputName);
			if(!inputIdx)
				return false;
			auto &input = inputs[*inputIdx];
			return input.SetValue<T>(val);
		}
		template<typename T>
		bool GetInputValue(const std::string_view &inputName, T &outVal)
		{
			auto inputIdx = node.FindInputIndex(inputName);
			if(!inputIdx)
				return false;
			auto &input = inputs[*inputIdx];
			return input.GetValue<T>(outVal);
		}
		void ClearInputValue(const std::string_view &inputName);
		bool Disconnect(const std::string_view &inputName);
		bool Disconnect(const std::string_view &outputName, GraphNode &linkTarget, const std::string_view &inputName);
		bool CanLink(const std::string_view &outputName, GraphNode &linkTarget, const std::string_view &inputName) const;
		bool Link(const std::string_view &outputName, GraphNode &linkTarget, const std::string_view &inputName);

		bool IsOutputLinked(const std::string_view &name) const;
		std::optional<size_t> FindOutputIndex(const std::string_view &name) const;
		std::optional<size_t> FindInputIndex(const std::string_view &name) const;
		InputTest *FindInput(const std::string_view &name);
		OutputTest *FindOutput(const std::string_view &name);
		const InputTest *FindInput(const std::string_view &name) const { return const_cast<GraphNode *>(this)->FindInput(name); }
		const OutputTest *FindOutput(const std::string_view &name) const { return const_cast<GraphNode *>(this)->FindOutput(name); }
		InputTest *GetInput(size_t index);
		OutputTest *GetOutput(size_t index);
		const InputTest *GetInput(size_t index) const { return const_cast<GraphNode *>(this)->GetInput(index); }
		const OutputTest *GetOutput(size_t index) const { return const_cast<GraphNode *>(this)->GetOutput(index); }

		std::string GetOutputVarName(size_t outputIdx) const;
		std::string GetOutputVarName(const std::string_view &name) const;

		bool Save(udm::LinkedPropertyWrapper &prop) const;
		bool LoadFromAssetData(udm::LinkedPropertyWrapper &prop, std::vector<SocketLink> &outLinks, std::string &outErr);

		Graph &graph;
		uint32_t nodeIndex = std::numeric_limits<uint32_t>::max();
		std::string m_name;
		std::optional<std::string> m_displayName {};
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
