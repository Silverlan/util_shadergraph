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

export module pragma.shadergraph:node;

import :socket;
#pragma optimize("", off)
export namespace pragma::shadergraph {
	class Graph;
	struct GraphNode;
	class Node {
	  public:
		using Ptr = std::shared_ptr<Node>;
		Node(const std::string_view &type);
		virtual ~Node();

		const std::string_view &GetType() const;
		std::string GetInputNameOrValue(const GraphNode &instance, uint32_t inputIdx) const;
		std::string GetInputNameOrValue(const GraphNode &instance, const std::string_view &inputName) const;
		std::string Evaluate(const Graph &graph, const GraphNode &instance) const;
		template<typename TEnum>
		    requires(std::is_enum_v<TEnum>)
		void AddSocketEnum(const std::string &name, TEnum defaultVal)
		{
			m_inputs.emplace_back(name, SocketType::Enum);
			using T = std::underlying_type_t<TEnum>;
			// static_assert(std::numeric_limits<T>::lowest() >= std::numeric_limits<udm::Int32>::lowest() && std::numeric_limits<T>::max() <= std::numeric_limits<udm::Int32>::max(), "Underlying enum type must be compatible with int32.");
			m_inputs.back().defaultValue.Set(static_cast<udm::Int32>(defaultVal));
		}
		void AddSocket(const std::string &name, SocketType type, auto defaultVal);
		void AddInput(const std::string &name, SocketType type, auto defaultVal);
		void AddOutput(const std::string &name, SocketType type);

		std::optional<size_t> FindOutputIndex(const std::string_view &name) const;
		std::optional<size_t> FindInputIndex(const std::string_view &name) const;
		const std::vector<Socket> &GetInputs() const;
		const std::vector<Socket> &GetOutputs() const;
		const Socket *GetInput(size_t index) const;
		const Socket *GetOutput(size_t index) const;
	  protected:
		std::string GetUniqueVarName() const;
		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const = 0;

		std::string_view m_type;
		std::vector<Socket> m_inputs;
		std::vector<Socket> m_outputs;
	};

	void Node::AddSocket(const std::string &name, SocketType type, auto defaultVal)
	{
		m_inputs.emplace_back(name, type);
		m_inputs.back().defaultValue.Set(defaultVal);
	}
	void Node::AddInput(const std::string &name, SocketType type, auto defaultVal)
	{
		m_inputs.emplace_back(name, type);
		m_inputs.back().defaultValue.Set(defaultVal);
	}
};
