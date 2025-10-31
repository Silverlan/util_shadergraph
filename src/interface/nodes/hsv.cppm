// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


export module pragma.shadergraph:nodes.hsv;

import :node;
export namespace pragma::shadergraph {

	class HsvNode : public Node {
	  public:
		static constexpr const char *IN_HUE = "hue";
		static constexpr const char *IN_SATURATION = "saturation";
		static constexpr const char *IN_VALUE = "value";
		static constexpr const char *IN_FAC = "fac";
		static constexpr const char *IN_COLOR = "color";

		static constexpr const char *OUT_COLOR = "color";

		HsvNode(const std::string_view &type);

		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const override;
	};
};
