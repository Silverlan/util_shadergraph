/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string_view>
#include <string>

export module pragma.shadergraph:nodes.mix;

import :node;
export namespace pragma::shadergraph {

	class MixNode : public Node {
	  public:
		enum class Type : uint32_t {
			Mix = 0,
			Add,
			Multiply,
			Screen,
			Overlay,
			Subtract,
			Divide,
			Difference,
			Darken,
			Lighten,
			Dodge,
			Burn,
			Hue,
			Saturation,
			Value,
			Color,
			SoftLight,
			LinearLight,
			Exclusion,
		};

		static constexpr const char *IN_TYPE = "type";
		static constexpr const char *IN_CLAMP = "clamp";
		static constexpr const char *IN_FAC = "fac";
		static constexpr const char *IN_COLOR1 = "color1";
		static constexpr const char *IN_COLOR2 = "color2";

		static constexpr const char *OUT_COLOR = "color";

		MixNode(const std::string_view &type);

		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const override;
	};
};
