/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string_view>
#include <sstream>
#include <mathutil/uvec.h>

module pragma.shadergraph;

import :nodes.hsv;

using namespace pragma::shadergraph;

SeparateHsv::SeparateHsv(const std::string_view &type) : Node {type}
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
