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
	auto reg = std::make_shared < NodeRegistry>();
	reg->RegisterNode<MathNode>("math");

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
	//bool Link(const char *outputName, GraphNode &linkTarget, const char *inputName)
	//
	// TODO: Apply operation?

	// Add nodes to the graph
	// Connect nodes (this example does not fully implement connection and evaluation logic)
	//graph.ConnectNodes(mathNode1, 0, mathNode2, 1); // Connect output of mathNode1 to input of mathNode2

	// Generate GLSL code
	//std::string glslCode = graph.GenerateGLSL();
	//std::cout << "Generated GLSL:\n" << glslCode << std::endl;
}

Graph::Graph(const std::shared_ptr<NodeRegistry> &nodeReg) : m_nodeRegistry {nodeReg} {}
std::shared_ptr<GraphNode> Graph::GetNode(const std::string &name)
{
	auto it = m_nameToNodeIndex.find(name);
	if(it == m_nameToNodeIndex.end())
		return nullptr;
	return m_nodes[it->second];
}
std::shared_ptr<GraphNode> Graph::AddNode(const std::string &type)
{
	auto node = m_nodeRegistry->GetNode(type);
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

	auto inst = std::make_shared<GraphNode>(*this, *node, name);
	m_nodes.push_back(inst);
	m_nameToNodeIndex[name] = m_nodes.size() - 1;
	return inst;
}

std::vector<GraphNode *> Graph::topologicalSort(const std::vector<std::shared_ptr<GraphNode>> &nodes)
{
	std::unordered_map<GraphNode *, int> in_degree;
	std::unordered_map<GraphNode *, std::vector<GraphNode *>> adj_list;

	// Step 1: Initialize in-degree and adjacency list
	for(const auto &node : nodes) {
		in_degree[node.get()] = 0; // Initialize in-degree count
	}

	for(const auto &node : nodes) {
		for(auto &output : node->outputs) {
			for(auto *input_link : output.links) {
				GraphNode *dependent_node = input_link->parent;

				// Populate adjacency list and in-degree count
				adj_list[node.get()].push_back(dependent_node);
				in_degree[dependent_node]++;
			}
		}
	}

	// Step 2: Collect all nodes with zero in-degree
	std::queue<GraphNode *> zero_in_degree_queue;
	for(auto &[node, degree] : in_degree) {
		if(degree == 0) {
			zero_in_degree_queue.push(node);
		}
	}

	// Step 3: Process the nodes in topological order
	std::vector<GraphNode *> sorted_nodes;
	while(!zero_in_degree_queue.empty()) {
		GraphNode *node = zero_in_degree_queue.front();
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
		std::cout << "Node " << node->GetName() << " (" << node->node.GetType() << ")" << "\n";

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

bool Graph::Load(udm::LinkedPropertyWrapper &prop, std::string &outErr)
{
	/*if(data.GetAssetType() != PSG_IDENTIFIER) {
		outErr = "Incorrect format!";
		return false;
	}

	const auto udm = *data;
	auto version = data.GetAssetVersion();
	if(version < 1) {
		outErr = "Invalid version!";
		return false;
	}*/

	auto udmNodes = prop["nodes"];
	auto numNodes = udmNodes.GetSize();
	std::vector<GraphNode::SocketLink> links;
	for(size_t idx = 0; idx < numNodes; ++idx) {
		auto udmNode = udmNodes[idx];
		std::string type;
		udmNode["type"] >> type;

		auto node = m_nodeRegistry->GetNode(type);
		if(!node) {
			outErr = "Unknown node type '" + type + "'!";
			return false;
		}

		std::string name;
		udmNode["name"] >> name;
		auto inst = std::make_shared<GraphNode>(*this, *node, name);
		if(!inst->LoadFromAssetData(udmNode, links, outErr))
			return false;
		if(m_nameToNodeIndex.find(name) != m_nameToNodeIndex.end()) {
			outErr = "Multiple nodes with name '" + name + "'. This is not allowed!";
			return false;
		}
		m_nodes.push_back(inst);
		m_nameToNodeIndex[name] = m_nodes.size() - 1;
	}

	for(auto &link : links) {
		auto node = GetNode(link.outputNode);
		if(!node)
			return false;
		auto *output = node->FindOutput(link.outputName);
		if(!output)
			return false;
		if(!node->Link(link.outputName, *link.inputSocket->parent, link.inputSocket->GetSocket().name))
			return false;
	}

	return true;
}

bool Graph::Save(udm::AssetDataArg outData, std::string &outErr) const
{
	outData.SetAssetType(PSG_IDENTIFIER);
	outData.SetAssetVersion(PSG_VERSION);
	auto udm = *outData;

	auto udmNodes = udm.AddArray("nodes", m_nodes.size());
	for(size_t idx = 0; idx < m_nodes.size(); ++idx) {

		auto &node = m_nodes[idx];
		auto udmGraphNode = udmNodes[idx];
		node->Save(udmGraphNode);
	}
	return true;
}

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
		glslCode << "// " << node->GetName() << " (" << (*node)->GetType() << ")\n";
		glslCode << node->node.Evaluate(*this, *node);
		glslCode << "\n";
	}

	return glslCode.str();
}
