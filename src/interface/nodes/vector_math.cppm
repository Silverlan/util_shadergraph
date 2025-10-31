// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


export module pragma.shadergraph:nodes.vector_math;

import :node;

export namespace pragma::shadergraph {

	class VectorMathNode : public Node {
	  public:
		enum class Operation : uint32_t {
			Add,
			Subtract,
			Multiply,
			Divide,

			CrossProduct,
			Project,
			Reflect,
			DotProduct,

			Distance,
			Length,
			Scale,
			Normalize,

			Snap,
			Floor,
			Ceil,
			Modulo,
			Fraction,
			Absolute,
			Minimum,
			Maximum,
			Wrap,
			Sine,
			Cosine,
			Tangent,
			Refract,
			FaceForward,
			MultiplyAdd,
		};

		static constexpr const char *IN_OPERATION = "operation";
		static constexpr const char *IN_VECTOR1 = "vector1";
		static constexpr const char *IN_VECTOR2 = "vector2";
		static constexpr const char *IN_VECTOR3 = "vector3";

		static constexpr const char *OUT_VALUE = "value";
		static constexpr const char *OUT_VECTOR = "vector";

		VectorMathNode(const std::string_view &type);

		virtual std::string DoEvaluate(const Graph &graph, const GraphNode &instance) const override;
	};
};
