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

import :nodes.sepia_tone;

using namespace pragma::shadergraph;

SepiaToneNode::SepiaToneNode(const std::string_view &type) : Node {type, CATEGORY_COLOR}
{
	AddInput(IN_COLOR, DataType::Color, Vector3 {0.f, 0.f, 0.f});

	AddOutput(OUT_COLOR, DataType::Color);
}

std::string SepiaToneNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto color = gn.GetInputNameOrValue(IN_COLOR);

	auto prefix = gn.GetBaseVarName() + "_";
	auto gray = prefix + "gray";
	code << "float " << gray << " = dot(" << color << ", vec3(0.3, 0.59, 0.11));\n";
	code << gn.GetGlslOutputDeclaration(OUT_COLOR) << " = ";
	code << "vec3(\n";
	code << "\tmin(" << gray << " *0.393 + " << color << ".g *0.769 + " << color << ".b *0.189, 1.0),\n";
	code << "\tmin(" << gray << " *0.349 + " << color << ".g *0.686 + " << color << ".b *0.168, 1.0),\n";
	code << "\tmin(" << gray << " *0.272 + " << color << ".g *0.534 + " << color << ".b *0.131, 1.0)\n";
	code << ");\n";
	return code.str();
}
