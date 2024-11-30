/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

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
