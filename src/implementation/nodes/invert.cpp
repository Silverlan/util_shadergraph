// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


module pragma.shadergraph;

import :nodes.invert;

using namespace pragma::shadergraph;

InvertNode::InvertNode(const std::string_view &type) : Node {type, CATEGORY_COLOR}
{
	AddInput(IN_FAC, DataType::Float, 1.f);
	AddInput(IN_COLOR, DataType::Color, Vector3 {0.f, 0.f, 0.f});

	AddOutput(OUT_COLOR, DataType::Color);
}

std::string InvertNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto fac = gn.GetInputNameOrValue(IN_FAC);
	auto color = gn.GetInputNameOrValue(IN_COLOR);

	code << gn.GetGlslOutputDeclaration(OUT_COLOR) << " = ";
	code << "mix(" << color << ", vec3(1.0) - " << color << ", " << fac << ");\n";
	return code.str();
}
