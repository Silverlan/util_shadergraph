// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string_view>
#include <sstream>

module pragma.shadergraph;

import :nodes.vector_math;

using namespace pragma::shadergraph;

VectorMathNode::VectorMathNode(const std::string_view &type) : Node {type, CATEGORY_VECTOR_MATH}
{
	AddSocketEnum<Operation>(IN_OPERATION, Operation::Add);
	AddInput(IN_VECTOR1, DataType::Vector, Vector3 {0.f, 0.f, 0.f});
	AddInput(IN_VECTOR2, DataType::Vector, Vector3 {0.f, 0.f, 0.f});
	AddInput(IN_VECTOR3, DataType::Vector, Vector3 {0.f, 0.f, 0.f});

	AddOutput(OUT_VALUE, DataType::Float);
	AddOutput(OUT_VECTOR, DataType::Vector);

	AddModuleDependency("math");
}

static const char *get_output_name(pragma::shadergraph::VectorMathNode::Operation op)
{
	switch(op) {
	case pragma::shadergraph::VectorMathNode::Operation::DotProduct:
	case pragma::shadergraph::VectorMathNode::Operation::Distance:
	case pragma::shadergraph::VectorMathNode::Operation::Length:
		return pragma::shadergraph::VectorMathNode::OUT_VALUE;
	}
	return pragma::shadergraph::VectorMathNode::OUT_VECTOR;
}

std::string VectorMathNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	auto v1 = gn.GetInputNameOrValue(IN_VECTOR1);
	auto v2 = gn.GetInputNameOrValue(IN_VECTOR2);
	auto v3 = gn.GetInputNameOrValue(IN_VECTOR3);
	auto op = *gn.GetConstantInputValue<Operation>(IN_OPERATION);

	code << gn.GetGlslOutputDeclaration(OUT_VALUE) << " = 0.0;\n";
	code << gn.GetGlslOutputDeclaration(OUT_VECTOR) << " = vec3(0.0, 0.0, 0.0);\n";

	code << gn.GetOutputVarName(get_output_name(op)) << " = ";
	switch(op) {
	case Operation::Add:
		code << v1 << " +" << v2;
		break;
	case Operation::Subtract:
		code << v1 << " -" << v2;
		break;
	case Operation::Multiply:
		code << v1 << " *" << v2;
		break;
	case Operation::Divide:
		code << v1 << " /" << v2;
		break;
	case Operation::CrossProduct:
		code << "cross(" << v1 << ", " << v2 << ")";
		break;
	case Operation::DotProduct:
		code << "dot(" << v1 << ", " << v2 << ")";
		break;
	case Operation::Scale:
		code << v1 << " *" << v2;
		break;
	case Operation::Distance:
		code << "distance(" << v1 << ", " << v2 << ")";
		break;
	case Operation::Length:
		code << "length(" << v1 << ")";
		break;
	case Operation::Normalize:
		code << "normalize(" << v1 << ")";
		break;
	case Operation::Reflect:
		code << "reflect(" << v1 << ", " << v2 << ")";
		break;
	case Operation::Project:
		code << "project(" << v1 << ", " << v2 << ")";
		break;
	case Operation::Snap:
		code << "floor(" << v1 << " /" << v2 << ") *" << v2;
		break;
	case Operation::Floor:
		code << "floor(" << v1 << ")";
		break;
	case Operation::Ceil:
		code << "ceil(" << v1 << ")";
		break;
	case Operation::Modulo:
		code << "mod(" << v1 << ", " << v2 << ")";
		break;
	case Operation::Fraction:
		code << v1 << " -floor(" << v1 << ")";
		break;
	case Operation::Absolute:
		code << "abs(" << v1 << ")";
		break;
	case Operation::Minimum:
		code << "min(" << v1 << ", " << v2 << ")";
		break;
	case Operation::Maximum:
		code << "max(" << v1 << ", " << v2 << ")";
		break;
	case Operation::Wrap:
		code << "wrap(" << v1 << ", " << v2 << ", " << v3 << ")";
		break;
	case Operation::Sine:
		code << "sin(" << v1 << ")";
		break;
	case Operation::Cosine:
		code << "cos(" << v1 << ")";
		break;
	case Operation::Tangent:
		code << "tan(" << v1 << ")";
		break;
	case Operation::Refract:
		code << "refract(" << v1 << ", " << v2 << ", " << v3 << ")";
		break;
	case Operation::FaceForward:
		code << "faceforward(" << v1 << ", " << v2 << ", " << v3 << ")";
		break;
	case Operation::MultiplyAdd:
		code << v1 << " *" << v2 << " +" << v3;
		break;
	default:
		throw std::runtime_error("Unknown operation in VectorMathNode::DoEvaluate");
	}
	code << ";\n";

	return code.str();
}
