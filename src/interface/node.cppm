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

export module pragma.shadergraph:node;

import :socket;
#pragma optimize("", off)
export namespace pragma::shadergraph {
	constexpr std::string_view CATEGORY_INPUT_PARAMETER = "input_parameter";
	constexpr std::string_view CATEGORY_INPUT_SYSTEM = "input_system";
	constexpr std::string_view CATEGORY_MATH = "math";
	constexpr std::string_view CATEGORY_VECTOR_MATH = "vector_math";
	constexpr std::string_view CATEGORY_COLOR = "color";
	constexpr std::string_view CATEGORY_TEXTURE = "texture";
	constexpr std::string_view CATEGORY_SHADER = "shader";
	constexpr std::string_view CATEGORY_SCENE = "scene";
	constexpr std::string_view CATEGORY_ENVIRONMENT = "environment";
	constexpr std::string_view CATEGORY_OUTPUT = "output";
	constexpr std::string_view CATEGORY_UTILITY = "utility";

	class Graph;
	struct GraphNode;
	class Node {
	  public:
		Node(const std::string_view &type, const std::string_view &category);
		Node(const Node &) = delete;
		Node &operator=(const Node &) = delete;
		virtual ~Node();

		const std::string_view &GetType() const;
		const std::string_view &GetCategory() const;
		std::string GetConstantValue(const GraphNode &instance, uint32_t inputIdx) const;
		std::string GetConstantValue(const GraphNode &instance, const std::string_view &inputName) const;
		std::string GetInputNameOrValue(const GraphNode &instance, uint32_t inputIdx) const;
		std::string GetInputNameOrValue(const GraphNode &instance, const std::string_view &inputName) const;
		const std::vector<std::string> &GetModuleDependencies() const { return m_dependencies; }

		virtual void Expand(Graph &graph, GraphNode &gn) const {}

		std::string Evaluate(const Graph &graph, const GraphNode &instance) const;
		std::string EvaluateResourceDeclarations(const Graph &graph, const GraphNode &instance) const;
		template<typename TEnum>
		    requires(std::is_enum_v<TEnum>)
		void AddSocketEnum(const std::string &name, TEnum defaultVal, bool linkable = false)
		{
			m_inputs.emplace_back(name, DataType::Enum);
			using T = std::underlying_type_t<TEnum>;
			// static_assert(std::numeric_limits<T>::lowest() >= std::numeric_limits<udm::Int32>::lowest() && std::numeric_limits<T>::max() <= std::numeric_limits<udm::Int32>::max(), "Underlying enum type must be compatible with int32.");
			m_inputs.back().defaultValue.Set(static_cast<udm::Int32>(defaultVal));
			m_inputs.back().enumSet = EnumSet::Create<TEnum>();
			if(linkable)
				m_inputs.back().flags |= Socket::Flags::Linkable;
		}
		template<typename T>
		Socket &AddSocket(const std::string &name, DataType type, T defaultVal, float min = 0.f, float max = 1.f);
		template<typename T>
		Socket &AddInput(const std::string &name, DataType type, T defaultVal, float min = 0.f, float max = 1.f);
		Socket &AddOutput(const std::string &name, DataType type);

		std::optional<size_t> FindOutputIndex(const std::string_view &name) const;
		std::optional<size_t> FindInputIndex(const std::string_view &name) const;
		const std::vector<Socket> &GetInputs() const;
		const std::vector<Socket> &GetOutputs() const;
		const Socket *GetInput(size_t index) const;
		const Socket *GetOutput(size_t index) const;

		std::string GetGlslOutputDeclaration(const GraphNode &instance, uint32_t outputIdx) const;
		std::string GetGlslOutputDeclaration(const GraphNode &instance, const std::string_view &name) const;
	  protected:
		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const = 0;
		virtual std::string DoEvaluateResourceDeclarations(const Graph &graph, const GraphNode &instance) const { return ""; }
		void AddModuleDependency(const std::string &name) { m_dependencies.push_back(name); }

		std::string_view m_type;
		std::string_view m_category;
		std::vector<Socket> m_inputs;
		std::vector<Socket> m_outputs;
		std::vector<std::string> m_dependencies;
	  private:
		Socket &AddSocket(const std::string &name, DataType type, float min, float max);
	};

	template<typename T>
	Socket &Node::AddSocket(const std::string &name, DataType type, T defaultVal, float min, float max)
	{
		if constexpr(std::is_floating_point_v<T> || std::is_integral_v<T>) {
			if(defaultVal < min)
				min = defaultVal;
			if(defaultVal > max)
				max = defaultVal;
		}
		auto &socket = AddSocket(name, type, min, max);
		socket.defaultValue.Set(defaultVal);
		return socket;
	}
	template<typename T>
	Socket &Node::AddInput(const std::string &name, DataType type, T defaultVal, float min, float max)
	{
		auto &socket = AddSocket<T>(name, type, defaultVal, min, max);
		socket.flags |= Socket::Flags::Linkable;
		return socket;
	}
};
