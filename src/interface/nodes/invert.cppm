// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.shadergraph:nodes.invert;

import :node;
export namespace pragma::shadergraph {

	class InvertNode : public Node {
	  public:
		static constexpr const char *IN_FAC = "fac";
		static constexpr const char *IN_COLOR = "color";

		static constexpr const char *OUT_COLOR = "color";

		InvertNode(const std::string_view &type);

		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const override;
	};
};
