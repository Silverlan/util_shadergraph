// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


export module pragma.shadergraph:nodes.value;

import :node;
export namespace pragma::shadergraph {

	class ValueNode : public Node {
	  public:
		static constexpr const char *CONST_VALUE = "value";

		static constexpr const char *OUT_VALUE = "value";

		ValueNode(const std::string_view &type);

		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const override;
	};
};
