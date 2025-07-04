// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

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
	struct InputSocket;
	struct OutputSocket {
		OutputSocket(GraphNode &node, uint32_t index);
		const Socket &GetSocket() const;
		GraphNode *parent = nullptr;
		std::vector<InputSocket *> links;
		uint32_t outputIndex = std::numeric_limits<uint32_t>::max();
	};

	struct InputSocket {
		InputSocket(GraphNode &node, uint32_t index);
		~InputSocket();
		GraphNode *parent = nullptr;
		OutputSocket *link = nullptr;
		uint32_t inputIndex = std::numeric_limits<uint32_t>::max();

		const Socket &GetSocket() const;
		template<typename T>
		bool SetValue(const T &val);
		template<typename T>
		bool GetValue(T &outVal) const;
		void ClearValue();
		bool HasValue() const;
	  private:
		Value value;
	};

	class Graph;
	struct GraphNode {
		struct SocketLink {
			std::string outputNode;
			std::string outputName;

			InputSocket *inputSocket;
		};

		const Node &node;
		std::vector<InputSocket> inputs;
		std::vector<OutputSocket> outputs;
		GraphNode(Graph &graph, Node &node);
		GraphNode(const GraphNode &other) = delete;
		GraphNode(Graph &graph, const GraphNode &other);
		GraphNode &operator=(const GraphNode &) = delete;
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
		bool GetInputValue(const std::string_view &inputName, T &outVal) const
		{
			auto inputIdx = node.FindInputIndex(inputName);
			if(!inputIdx)
				return false;
			auto &input = inputs[*inputIdx];
			return input.GetValue<T>(outVal);
		}
		void Relink(uint32_t outputIdx, GraphNode &newNode, uint32_t newNodeOutputIdx);
		void Relink(const std::string_view &outputName, GraphNode &newNode, const std::string_view &newNodeOutputName);

		void PropagateInputSocket(uint32_t inputIdx, GraphNode &otherNode, uint32_t otherNodeInputIdx);
		void PropagateInputSocket(const std::string_view &inputName, GraphNode &otherNode, const std::string_view &otherNodeInputName);

		bool IsInputLinked(uint32_t inputIdx) const;
		bool IsInputLinked(const std::string_view &name) const;

		std::string GetConstantValue(uint32_t inputIdx) const { return node.GetConstantValue(*this, inputIdx); }
		std::string GetConstantValue(const std::string_view &inputName) const { return node.GetConstantValue(*this, inputName); }
		std::string GetInputNameOrValue(uint32_t inputIdx) const { return node.GetInputNameOrValue(*this, inputIdx); }
		std::string GetInputNameOrValue(const std::string_view &inputName) const { return node.GetInputNameOrValue(*this, inputName); }

		const InputSocket *FindInputSocket(const std::string_view &inputName) const
		{
			auto &nodeInputs = node.GetInputs();
			auto it = std::find_if(nodeInputs.begin(), nodeInputs.end(), [&inputName](const Socket &socket) { return socket.name == inputName; });
			if(it == nodeInputs.end())
				return nullptr;
			auto idx = it - nodeInputs.begin();
			return &inputs[idx];
		}

		template<typename T>
		std::optional<T> GetConstantInputValue(uint32_t inputIdx) const
		{
			auto &input = inputs.at(inputIdx);
			if(input.link && input.link->parent)
				return {};
			return visit(input.GetSocket().type, [&input](auto tag) -> std::optional<T> {
				using TTo = T;
				using TFrom = typename decltype(tag)::type;
				if constexpr(udm::is_convertible<TFrom, TTo>()) {
					TFrom v;
					if(!input.GetValue(v))
						return {};
					return udm::convert<TFrom, TTo>(v);
				}
				return {};
			});
		}
		template<typename T>
		std::optional<T> GetConstantInputValue(const std::string_view &inputName) const
		{
			if constexpr(std::is_enum_v<T>) {
				auto v = GetConstantInputValue<typename std::underlying_type<T>::type>(inputName);
				return v ? static_cast<T>(*v) : std::optional<T> {};
			}
			else {
				auto &inputs = node.GetInputs();
				auto it = std::find_if(inputs.begin(), inputs.end(), [&inputName](const Socket &socket) { return socket.name == inputName; });
				if(it == inputs.end())
					throw std::invalid_argument {"No input named '" + std::string {inputName} + "' exists!"};
				return GetConstantInputValue<T>(it - inputs.begin());
			}
			// Unreachable
			return {};
		}

		std::string GetGlslOutputDeclaration(uint32_t outputIdx) const { return node.GetGlslOutputDeclaration(*this, outputIdx); }
		std::string GetGlslOutputDeclaration(const std::string_view &name) const { return node.GetGlslOutputDeclaration(*this, name); }

		void ClearInputValue(const std::string_view &inputName);
		void DisconnectAll();
		void DisconnectInputs();
		void DisconnectOutputs();
		void DisconnectOutputs(uint32_t outputIdx);
		bool Disconnect(const std::string_view &inputName);
		bool Disconnect(uint32_t inputIdx);
		bool Disconnect(const std::string_view &outputName, GraphNode &linkTarget, const std::string_view &inputName);
		bool Disconnect(uint32_t outputIdx, GraphNode &linkTarget, uint32_t inputIdx);
		bool CanLink(const std::string_view &outputName, GraphNode &linkTarget, const std::string_view &inputName) const;
		bool CanLink(uint32_t outputIdx, GraphNode &linkTarget, uint32_t inputIdx) const;
		bool Link(const std::string_view &outputName, GraphNode &linkTarget, const std::string_view &inputName, std::string *optOutErr = nullptr);
		bool Link(uint32_t outputIdx, GraphNode &linkTarget, uint32_t inputIdx, std::string *optOutErr = nullptr);

		bool IsOutputLinked(const std::string_view &name) const;
		std::optional<size_t> FindOutputIndex(const std::string_view &name) const;
		std::optional<size_t> FindInputIndex(const std::string_view &name) const;
		InputSocket *FindInput(const std::string_view &name);
		OutputSocket *FindOutput(const std::string_view &name);
		const InputSocket *FindInput(const std::string_view &name) const { return const_cast<GraphNode *>(this)->FindInput(name); }
		const OutputSocket *FindOutput(const std::string_view &name) const { return const_cast<GraphNode *>(this)->FindOutput(name); }
		InputSocket *GetInput(size_t index);
		OutputSocket *GetOutput(size_t index);
		const InputSocket *GetInput(size_t index) const { return const_cast<GraphNode *>(this)->GetInput(index); }
		const OutputSocket *GetOutput(size_t index) const { return const_cast<GraphNode *>(this)->GetOutput(index); }

		std::string GetBaseVarName() const;
		std::string GetVarName(const std::string &var) const;
		std::string GetOutputVarName(size_t outputIdx) const;
		std::string GetOutputVarName(const std::string_view &name) const;

		const Vector2 &GetPos() const { return m_pos; }
		void SetPos(const Vector2 &pos) { m_pos = pos; }

		bool Save(udm::LinkedPropertyWrapper &prop) const;
		bool LoadFromAssetData(udm::LinkedPropertyWrapper &prop, std::vector<SocketLink> &outLinks, std::string &outErr);

		friend Graph;
		Graph &graph;
		uint32_t nodeIndex = std::numeric_limits<uint32_t>::max();
		std::string m_name;
		std::optional<std::string> m_displayName {};
	  private:
		void SetName(const std::string &name) { m_name = name; }
		Vector2 m_pos {};
	};

	template<typename T>
	bool InputSocket::SetValue(const T &val)
	{
		return value.Set<T>(val);
	}
	template<typename T>
	bool InputSocket::GetValue(T &outVal) const
	{
		if(value)
			return value.Get<T>(outVal);
		return GetSocket().defaultValue.Get<T>(outVal);
	}
};
