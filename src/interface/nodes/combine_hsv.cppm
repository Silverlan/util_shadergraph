// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

export module pragma.shadergraph:nodes.combine_hsv;

import :node;
export namespace pragma::shadergraph {

	class CombineHsvNode : public Node {
	  public:
		static constexpr const char *IN_H = "h";
		static constexpr const char *IN_S = "s";
		static constexpr const char *IN_V = "v";

		static constexpr const char *OUT_COLOR = "color";

		CombineHsvNode(const std::string_view &type);

		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const override;
	};
};
