// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.shadergraph:nodes.map_range_node;

import :node;
export namespace pragma::shadergraph {

	class MapRangeNode : public Node {
	  public:
		enum class Type : uint8_t {
			Linear = 0,
			Stepped,
			Smoothstep,
			Smootherstep,
		};

		static constexpr const char *CONST_TYPE = "type";

		static constexpr const char *IN_VALUE = "value";
		static constexpr const char *IN_FROM_MIN = "from_min";
		static constexpr const char *IN_FROM_MAX = "from_max";
		static constexpr const char *IN_TO_MIN = "to_min";
		static constexpr const char *IN_TO_MAX = "to_max";
		static constexpr const char *IN_STEPS = "steps";
		static constexpr const char *IN_CLAMP = "clamp";

		static constexpr const char *OUT_RESULT = "result";

		MapRangeNode(const std::string_view &type);

		virtual void Expand(Graph &graph, GraphNode &gn) const override;
		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const override;
	};
};
