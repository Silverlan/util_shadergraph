// SPDX-FileCopyrightText: (c) 2025 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string_view>
#include <string>
#include <cinttypes>

export module pragma.shadergraph:nodes.emission;

import :node;
export namespace pragma::shadergraph {

	class EmissionNode : public Node {
	  public:
		enum class EmissionMode : uint32_t {
			Additive,
			HighlightBoost,
			Blend,
			Modulate,
			Screen,
			SoftLight,
			Multiply,
		};

		static constexpr const char *IN_COLOR = "color";
		static constexpr const char *IN_EMISSION_COLOR = "emissionColor";
		static constexpr const char *IN_EMISSION_ALPHA = "emissionAlpha";
		static constexpr const char *IN_BASE_COLOR = "baseColor";
		static constexpr const char *IN_EMISSION_MODE = "emissionMode";
		static constexpr const char *IN_EMISSION_FACTOR = "emissionFactor";

		static constexpr const char *OUT_COLOR = "color";
		static constexpr const char *OUT_EMISSION_COLOR = "emissionColor";

		EmissionNode(const std::string_view &type);

		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const override;
	};
};
