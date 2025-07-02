// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string_view>
#include <sstream>
#include <mathutil/uvec.h>

module pragma.shadergraph;

import :nodes.rgb_to_bw;

using namespace pragma::shadergraph;

RgbToBwNode::RgbToBwNode(const std::string_view &type) : Node {type, CATEGORY_COLOR}
{
	AddInput(IN_COLOR, DataType::Color, Vector3 {0.f, 0.f, 0.f});

	AddOutput(OUT_VAL, DataType::Float);
}

std::string RgbToBwNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto color = gn.GetInputNameOrValue(IN_COLOR);

	code << gn.GetGlslOutputDeclaration(OUT_VAL) << " = ";
	code << "dot(" << color << ", vec3(0.2126729f, 0.7151522f, 0.0721750f));\n"; // BT.709 Standard
	return code.str();
}
