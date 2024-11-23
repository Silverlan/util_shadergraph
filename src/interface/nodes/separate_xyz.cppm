/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string_view>
#include <string>

export module pragma.shadergraph:nodes.separate_xyz;

import :node;
#pragma optimize("", off)
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
