// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string_view>
#include <string>

export module pragma.shadergraph:nodes.separate_hsv;

import :node;
export namespace pragma::shadergraph {

	class SeparateHsv : public Node {
	  public:
		static constexpr const char *IN_COLOR = "color";

		static constexpr const char *OUT_H = "h";
		static constexpr const char *OUT_S = "s";
		static constexpr const char *OUT_V = "v";

		SeparateHsv(const std::string_view &type);

		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const override;
	};
};
