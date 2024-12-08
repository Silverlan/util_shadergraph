/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string_view>
#include <string>
#include <cinttypes>

export module pragma.shadergraph:nodes.clamp;

import :node;
export namespace pragma::shadergraph {

	class ClampNode : public Node {
	  public:
		enum class ClampType : uint8_t {
			MinMax = 0,
			Range,
		};

		static constexpr const char *CONST_CLAMP_TYPE = "clampType";

		static constexpr const char *IN_VALUE = "value";
		static constexpr const char *IN_MIN = "min";
		static constexpr const char *IN_MAX = "max";

		static constexpr const char *OUT_RESULT = "result";

		ClampNode(const std::string_view &type);

		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const override;
	};
};
