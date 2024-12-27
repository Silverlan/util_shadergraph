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

SepiaToneNode::SepiaToneNode(const std::string_view &type) : Node {type}
{
	AddInput(IN_COLOR, DataType::Color, Vector3 {0.f, 0.f, 0.f});

	AddOutput(OUT_COLOR, DataType::Color);
}

std::string SepiaToneNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto color = gn.GetInputNameOrValue(IN_COLOR);

	// See https://stackoverflow.com/a/9449159
	code << gn.GetGlslOutputDeclaration(OUT_COLOR) << " = ";
	code << "vec3(dot(" << color << ", vec3(0.393f, 0.769f, 0.189f)), ";
	code << "dot(" << color << ", vec3(0.349f, 0.686f, 0.168f)), ";
	code << "dot(" << color << ", vec3(0.272f, 0.534f, 0.131f)));\n";
	return code.str();
}
