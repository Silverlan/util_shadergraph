// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string_view>
#include <string>

export module pragma.shadergraph:nodes.separate_xyz;

import :node;

export namespace pragma::shadergraph {

	class SeparateXyzNode : public Node {
	  public:
		static constexpr const char *IN_VECTOR = "vector";

		static constexpr const char *OUT_X = "x";
		static constexpr const char *OUT_Y = "y";
		static constexpr const char *OUT_Z = "z";

		SeparateXyzNode(const std::string_view &type);

		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const override;
	};
};
