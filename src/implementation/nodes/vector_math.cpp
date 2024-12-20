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

import :nodes.vector_math;

using namespace pragma::shadergraph;

VectorMathNode::VectorMathNode(const std::string_view &type) : Node {type}
{
	AddSocketEnum<Operation>(IN_OPERATION, Operation::Add);
	AddInput(IN_VECTOR1, SocketType::Vector, Vector3 {0.f, 0.f, 0.f});
	AddInput(IN_VECTOR2, SocketType::Vector, Vector3 {0.f, 0.f, 0.f});
	AddInput(IN_VECTOR3, SocketType::Vector, Vector3 {0.f, 0.f, 0.f});

	AddOutput(OUT_VALUE, SocketType::Float);
	AddOutput(OUT_VECTOR, SocketType::Vector);

	AddModuleDependency("math");
}

std::string VectorMathNode::DoEvaluate(const Graph &graph, const GraphNode &gn) const
{
	std::ostringstream code;
	code << gn.GetGlslOutputDeclaration(OUT_VALUE) << " = ";
	auto v1 = gn.GetInputNameOrValue(IN_VECTOR1);
	auto v2 = gn.GetInputNameOrValue(IN_VECTOR2);
	auto v3 = gn.GetInputNameOrValue(IN_VECTOR3);
	auto op = *gn.GetConstantInputValue<Operation>(IN_OPERATION);
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
