// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


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
