// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.shadergraph;

import :nodes.combine_hsv;

using namespace pragma::shadergraph;

CombineHsvNode::CombineHsvNode(const std::string_view &type) : Node {type, CATEGORY_COLOR}
{
	AddInput(IN_H, DataType::Float, 0.f);
	AddInput(IN_S, DataType::Float, 0.f);
	AddInput(IN_V, DataType::Float, 0.f);

	AddOutput(OUT_COLOR, DataType::Color);

	AddModuleDependency("color");
}

std::string CombineHsvNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto h = gn.GetInputNameOrValue(IN_H);
	auto s = gn.GetInputNameOrValue(IN_S);
	auto v = gn.GetInputNameOrValue(IN_V);

	auto color = gn.GetVarName("color");
	code << "vec3 " << color << " = hsv_to_rgb(vec3(" << h << ", " << s << ", " << v << "));\n";
	code << gn.GetGlslOutputDeclaration(OUT_COLOR) << " = " << color << ";\n";
	return code.str();
}
