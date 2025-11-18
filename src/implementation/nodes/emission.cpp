// SPDX-FileCopyrightText: (c) 2025 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.shadergraph;

import :nodes.emission;

using namespace pragma::shadergraph;

EmissionNode::EmissionNode(const std::string_view &type) : Node {type, CATEGORY_SHADER}
{
	AddInput(IN_COLOR, DataType::Color, Vector3 {0.f, 0.f, 0.f});
	AddInput(IN_EMISSION_COLOR, DataType::Color, Vector3 {0.f, 0.f, 0.f});
	AddInput(IN_EMISSION_ALPHA, DataType::Float, 1.f);
	AddInput(IN_BASE_COLOR, DataType::Color, Vector3 {0.f, 0.f, 0.f});
	AddInput(IN_EMISSION_FACTOR, DataType::Float, 1.f);
	AddSocketEnum<EmissionMode>(IN_EMISSION_MODE, EmissionMode::Additive);

	AddOutput(OUT_COLOR, DataType::Color);
	AddOutput(OUT_EMISSION_COLOR, DataType::Color);

	AddModuleDependency("emission");
}

std::string EmissionNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto emissionColor = gn.GetVarName("emissionColor");
	code << "vec3 " << emissionColor << " = ";
	code << "apply_emission_color(";
	code << "vec4(" << gn.GetInputNameOrValue(IN_COLOR) << ", 1.0)" << ", ";
	code << "vec4(" << gn.GetInputNameOrValue(IN_EMISSION_COLOR) << " *" << gn.GetInputNameOrValue(IN_EMISSION_FACTOR) << ", 1.0)" << ", ";
	code << "vec4(" << gn.GetInputNameOrValue(IN_BASE_COLOR) << ", 1.0)" << ", ";
	code << gn.GetInputNameOrValue(IN_EMISSION_MODE);
	code << ").rgb;\n";

	auto emissionAlpha = gn.GetInputNameOrValue(IN_EMISSION_ALPHA);
	code << gn.GetGlslOutputDeclaration(OUT_COLOR) << " = mix(" << gn.GetInputNameOrValue(IN_COLOR) << ", " << emissionColor << ", " << emissionAlpha << ");\n";

	code << gn.GetGlslOutputDeclaration(OUT_EMISSION_COLOR) << " = ";
	code << gn.GetInputNameOrValue(IN_EMISSION_COLOR) << " *" << gn.GetInputNameOrValue(IN_EMISSION_FACTOR) << " *" << emissionAlpha << ";\n";
	return code.str();
}
