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

import :nodes.combine_hsv;

using namespace pragma::shadergraph;

CombineHsvNode::CombineHsvNode(const std::string_view &type) : Node {type}
{
	AddInput(OUT_H, SocketType::Float, 0.f);
	AddInput(OUT_S, SocketType::Float, 0.f);
	AddInput(OUT_V, SocketType::Float, 0.f);

	AddOutput(IN_COLOR, SocketType::Color);

	AddModuleDependency("color");
}

std::string CombineHsvNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto h = gn.GetInputNameOrValue(OUT_H);
	auto s = gn.GetInputNameOrValue(OUT_S);
	auto v = gn.GetInputNameOrValue(OUT_V);

	auto color = gn.GetVarName("color");
	code << "vec3 " << color << " = hsv_to_rgb(vec3(" << h << ", " << s << ", " << v << "));\n";
	code << gn.GetGlslOutputDeclaration(IN_COLOR) << " = " << color << ";\n";
	return code.str();
}
