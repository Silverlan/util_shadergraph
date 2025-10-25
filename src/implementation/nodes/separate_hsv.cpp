// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string_view>
#include <sstream>

module pragma.shadergraph;

import :nodes.hsv;

using namespace pragma::shadergraph;

SeparateHsv::SeparateHsv(const std::string_view &type) : Node {type, CATEGORY_COLOR}
{
	AddInput(IN_COLOR, DataType::Color, Vector3 {0.f, 0.f, 0.f});

	AddOutput(OUT_H, DataType::Float);
	AddOutput(OUT_S, DataType::Float);
	AddOutput(OUT_V, DataType::Float);

	AddModuleDependency("color");
}

std::string SeparateHsv::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto color = gn.GetInputNameOrValue(IN_COLOR);

	auto hsv = gn.GetVarName("hsv");
	code << "vec3 " << hsv << " = rgb_to_hsv(" << color << ");\n";
	code << gn.GetGlslOutputDeclaration(OUT_H) << " = " << hsv << ".x;\n";
	code << gn.GetGlslOutputDeclaration(OUT_S) << " = " << hsv << ".y;\n";
	code << gn.GetGlslOutputDeclaration(OUT_V) << " = " << hsv << ".z;\n";
	return code.str();
}
