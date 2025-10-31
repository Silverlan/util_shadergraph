// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


export module pragma.shadergraph:nodes.sepia_tone;

import :node;
export namespace pragma::shadergraph {

	class SepiaToneNode : public Node {
	  public:
		static constexpr const char *IN_COLOR = "color";

		static constexpr const char *OUT_COLOR = "color";

		SepiaToneNode(const std::string_view &type);

		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const override;
	};
};
