// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string_view>
#include <string>

export module pragma.shadergraph:nodes.rgb_to_bw;

import :node;
export namespace pragma::shadergraph {

	class RgbToBwNode : public Node {
	  public:
		static constexpr const char *IN_COLOR = "color";

		static constexpr const char *OUT_VAL = "val";

		RgbToBwNode(const std::string_view &type);

		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const override;
	};
};
