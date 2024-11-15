/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <memory>
#include <string>
#include <iostream>
#include <queue>
#include <udm.hpp>

module pragma.shadergraph;

import :graph;
import :nodes.math;
#pragma optimize("", off)

using namespace pragma::shadergraph;

void Graph::Test()
{
	NodeRegistry reg {};
	reg.RegisterNode<MathNode>("math");

	// Create graph
	Graph graph {reg};
	auto node0 = graph.AddNode("math");
	auto node1 = graph.AddNode("math");

	node1->SetInputValue(MathNode::IN_OPERATION, MathNode::Operation::Multiply);
	node0->SetInputValue(MathNode::IN_VALUE1, 1.f);
	node0->Link(MathNode::OUT_VALUE, *node1, MathNode::IN_VALUE1);

	graph.DebugPrint();

	std::string glslCode = graph.GenerateGLSL();
	std::cout << "Generated GLSL:\n" << glslCode << std::endl;
	//bool Link(const char *outputName, NodeInstance &linkTarget, const char *inputName)
	//
	// TODO: Apply operation?

	// Add nodes to the graph
	// Connect nodes (this example does not fully implement connection and evaluation logic)
	//graph.ConnectNodes(mathNode1, 0, mathNode2, 1); // Connect output of mathNode1 to input of mathNode2

	// Generate GLSL code
	//std::string glslCode = graph.GenerateGLSL();
	//std::cout << "Generated GLSL:\n" << glslCode << std::endl;
}

Graph::Graph(const NodeRegistry &nodeReg) : m_nodeRegistry {nodeReg} {}
std::shared_ptr<NodeInstance> Graph::AddNode(const std::string &type)
{
	auto node = m_nodeRegistry.GetNode(type);
	if(!node)
		return nullptr;
	std::string name = type;
	size_t i = 1;
	for(;;) {
		auto namei = name + std::to_string(i);
		if(m_nameToNodeIndex.find(namei) == m_nameToNodeIndex.end()) {
			name = std::move(namei);
			break;
		}
		++i;
	}

	auto inst = std::make_shared<NodeInstance>(*node, name);
	m_nodes.push_back(inst);
	m_nameToNodeIndex[name] = m_nodes.size() - 1;
	return inst;
}

std::vector<NodeInstance *> Graph::topologicalSort(const std::vector<std::shared_ptr<NodeInstance>> &nodes)
{
	std::unordered_map<NodeInstance *, int> in_degree;
	std::unordered_map<NodeInstance *, std::vector<NodeInstance *>> adj_list;

	// Step 1: Initialize in-degree and adjacency list
	for(const auto &node : nodes) {
		in_degree[node.get()] = 0; // Initialize in-degree count
	}

	for(const auto &node : nodes) {
		for(auto &output : node->outputs) {
			for(auto *input_link : output.links) {
				NodeInstance *dependent_node = input_link->parent;

				// Populate adjacency list and in-degree count
				adj_list[node.get()].push_back(dependent_node);
				in_degree[dependent_node]++;
			}
		}
	}

	// Step 2: Collect all nodes with zero in-degree
	std::queue<NodeInstance *> zero_in_degree_queue;
	for(auto &[node, degree] : in_degree) {
		if(degree == 0) {
			zero_in_degree_queue.push(node);
		}
	}

	// Step 3: Process the nodes in topological order
	std::vector<NodeInstance *> sorted_nodes;
	while(!zero_in_degree_queue.empty()) {
		NodeInstance *node = zero_in_degree_queue.front();
		zero_in_degree_queue.pop();
		sorted_nodes.push_back(node);

		// Decrease in-degree for all neighbors
		for(auto *neighbor : adj_list[node]) {
			in_degree[neighbor]--;
			if(in_degree[neighbor] == 0) {
				zero_in_degree_queue.push(neighbor);
			}
		}
	}

	// Step 4: Check for cycles
	if(sorted_nodes.size() != nodes.size()) {
		throw std::runtime_error("Cycle detected in shader graph; topological sort not possible");
	}

	return sorted_nodes;
}

void Graph::DebugPrint()
{
	auto sortedNodes = topologicalSort(m_nodes);
	// Print the topologically sorted nodes in order
	std::cout << "Topological Sort Order:\n";
	for(const auto *node : sortedNodes) {
		std::cout << "Node " << node->GetName() << "\n";

		// Print Inputs
		std::cout << "  Inputs:\n";
		for(const auto &input : node->inputs) {
			std::cout << "    " << input.GetSocket().name;
			if(input.link && input.link->parent) {
				std::cout << " -> " << input.link->parent->GetName() << "[" << input.link->GetSocket().name << "]"
				          << "\n";
			}
			else {
				auto hasPrinted = visit(input.GetSocket().type, [&input](auto tag) -> bool {
					using T = typename decltype(tag)::type;
					T val;
					if(input.GetValue<T>(val)) {
						auto strVal = udm::convert<T, udm::String>(val);
						std::cout << ": " << strVal << std::endl;
						return true;
					}
					return false;
				});
				assert(hasPrinted);
				if(!hasPrinted)
					std::cout << "    No input link\n";
			}
		}

		// Print Outputs
		std::cout << "  Outputs:\n";
		for(const auto &output : node->outputs) {
			if(!output.links.empty()) {
				for(const auto *linked_input : output.links) {
					if(linked_input && linked_input->parent) {
						std::cout << "    To Node " << linked_input->parent->GetName() << "\n";
					}
				}
			}
			else {
				std::cout << "    No output links\n";
			}
		}

		std::cout << "-------------------\n";
	}
}

void Graph::FindInvalidLinks()
{
	// 1) Iterate graph
	// 2) Remember which nodes were iterated
	// 3) If output is iterated a second time
	//		- Mark as invalid and cancel iteration on this branch
}

//void ConnectNodes(Node::Ptr outputNode, int outputIndex, Node::Ptr inputNode, int inputIndex) { connections.emplace_back(outputNode, outputIndex, inputNode, inputIndex); }

std::string Graph::GenerateGLSL()
{
	auto sortedNodes = topologicalSort(m_nodes);

	uint32_t nodeIndex = 0;
	for(auto &node : sortedNodes)
		node->nodeIndex = nodeIndex++;

	// Sort nodes topologically (not implemented in this minimal example).
	std::ostringstream glslCode;

	// Traverse nodes and generate GLSL code for each
	for(const auto &node : sortedNodes) {
		glslCode << node->node.Evaluate(*this, *node);
	}

	return glslCode.str();
}
