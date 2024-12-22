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

HsvNode::HsvNode(const std::string_view &type) : Node {type}
{
	AddInput(IN_HUE, DataType::Float, 0.5f);
	AddInput(IN_SATURATION, DataType::Float, 1.f);
	AddInput(IN_VALUE, DataType::Float, 1.f);
	AddInput(IN_FAC, DataType::Float, 1.f);
	AddInput(IN_COLOR, DataType::Color, Vector3 {0.f, 0.f, 0.f});

	AddOutput(OUT_COLOR, DataType::Color);

	AddModuleDependency("color");
}

std::string HsvNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto hue = gn.GetInputNameOrValue(IN_HUE);
	auto saturation = gn.GetInputNameOrValue(IN_SATURATION);
	auto value = gn.GetInputNameOrValue(IN_VALUE);
	auto fac = gn.GetInputNameOrValue(IN_FAC);
	auto color = gn.GetInputNameOrValue(IN_COLOR);

	auto hsv = gn.GetVarName("hsv");
	code << "vec3 " << hsv << " = rgb_to_hsv(" << color << ");\n";
	code << hsv << ".x = mod(" << hsv << ".x + " << hue << " + 0.5, 1.0);\n";
	code << hsv << ".y = clamp(" << hsv << ".y * " << saturation << ", 0.0, 1.0);\n";
	code << hsv << ".z *= " << value << ";\n";
	code << color << " = hsv_to_rgb(" << hsv << ");\n";
	code << color << " = " << fac << " * " << color << " + (1.0 - " << fac << ") * " << color << ";\n";
	code << color << " = max(" << color << ", vec3(0.0));\n";
	code << gn.GetGlslOutputDeclaration(OUT_COLOR) << " = " << color << ";\n";
	return code.str();
}
