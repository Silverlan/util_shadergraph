// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string_view>
#include <sstream>

module pragma.shadergraph;

import :nodes.bright_contrast;

using namespace pragma::shadergraph;

BrightContrastNode::BrightContrastNode(const std::string_view &type) : Node {type, CATEGORY_COLOR}
{
	AddInput(IN_COLOR, DataType::Color, Vector3 {0.f, 0.f, 0.f});
	AddInput(IN_BRIGHT, DataType::Float, 0.f);
	AddInput(IN_CONTRAST, DataType::Float, 0.f);

	AddOutput(OUT_COLOR, DataType::Color);
}

std::string BrightContrastNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto color = gn.GetInputNameOrValue(IN_COLOR);
	auto brightness = gn.GetInputNameOrValue(IN_BRIGHT);
	auto contrast = gn.GetInputNameOrValue(IN_CONTRAST);

	auto a = gn.GetVarName("a");
	auto b = gn.GetVarName("b");

	code << "float " << a << " = 1.0f + " << contrast << ";\n";
	code << "float " << b << " = " << brightness << " - " << contrast << " * 0.5f;\n";

	code << gn.GetGlslOutputDeclaration(OUT_COLOR) << " = vec3(\n";
	code << "max(" << a << " * " << color << ".x + " << b << ", 0.0f),\n";
	code << "max(" << a << " * " << color << ".y + " << b << ", 0.0f),\n";
	code << "max(" << a << " * " << color << ".z + " << b << ", 0.0f)\n";
	code << ");\n";

	return code.str();
}
