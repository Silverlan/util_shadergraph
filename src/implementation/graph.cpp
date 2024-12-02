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
#include <unordered_map>
#include <stdexcept>
#include <unordered_set>
#include <sstream>
#include <udm.hpp>

module pragma.shadergraph;

import :graph;
import :nodes.math;
#pragma optimize("", off)

using namespace pragma::shadergraph;

void Graph::Test()
{
	auto reg = std::make_shared<NodeRegistry>();
	reg->RegisterNode<MathNode>("math");

	// Create graph
	Graph graph {reg};
	auto node0 = graph.AddNode("math");
	auto node1 = graph.AddNode("math");

	node1->SetInputValue(MathNode::IN_OPERATION, MathNode::Operation::Multiply);
	node0->SetInputValue(MathNode::IN_VALUE1, 1.f);
	node0->Link(MathNode::OUT_VALUE, *node1, MathNode::IN_VALUE1);

	graph.DebugPrint();

	std::ostringstream header, body;
	graph.GenerateGlsl(header, body);
	std::cout << "Generated GLSL:\n" << body.str() << std::endl;
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

Graph::Graph(const Graph &other) : m_nodeRegistry {other.m_nodeRegistry} { Merge(other); }

Graph::Graph(const std::shared_ptr<NodeRegistry> &nodeReg) : m_nodeRegistry {nodeReg} {}

void Graph::Merge(const Graph &other)
{
	m_nodes.reserve(m_nodes.size() + other.m_nodes.size());
	std::unordered_map<GraphNode *, GraphNode *> oldToNew;
	oldToNew.reserve(other.m_nodes.size());
	auto offset = m_nodes.size();
	for(auto &node : other.m_nodes) {
		auto newNode = std::make_shared<GraphNode>(*this, *node);
		AddNode(newNode);
		oldToNew[node.get()] = newNode.get();
	}
	for(size_t i = offset; i < m_nodes.size(); ++i) {
		auto *node = m_nodes[i].get();
		for(auto &output : node->outputs) {
			auto it = oldToNew.find(output.parent);
			if(it == oldToNew.end())
				throw std::runtime_error("Failed to find new parent node for output socket!");
			output.parent = it->second;
			for(size_t i = 0; i < output.links.size(); ++i) {
				auto *input = output.links[i];
				it = oldToNew.find(input->parent);
				if(it == oldToNew.end())
					throw std::runtime_error("Failed to find new node for output socket link!");
				output.links[i] = &it->second->inputs[input->inputIndex];
			}
		}
		for(auto &input : node->inputs) {
			auto it = oldToNew.find(input.parent);
			if(it == oldToNew.end())
				throw std::runtime_error("Failed to find new parent node for input socket!");
			input.parent = it->second;

			if(!input.link)
				continue;
			it = oldToNew.find(input.link->parent);
			if(it == oldToNew.end())
				throw std::runtime_error("Failed to find new node for input socket link!");
			input.link = &it->second->outputs[input.link->outputIndex];
		}
	}
}

std::shared_ptr<GraphNode> Graph::FindNodeByType(const std::string_view &type) const
{
	for(const auto &node : m_nodes) {
		if(node->node.GetType() == type)
			return node;
	}
	return nullptr;
}
std::shared_ptr<GraphNode> Graph::GetNode(const std::string &name)
{
	auto it = m_nameToNodeIndex.find(name);
	if(it == m_nameToNodeIndex.end())
		return nullptr;
	return m_nodes[it->second];
}
bool Graph::RemoveNode(const std::string &name)
{
	auto it = m_nameToNodeIndex.find(name);
	if(it == m_nameToNodeIndex.end())
		return false;
	auto idx = it->second;
	auto &node = m_nodes.at(idx);
	node->DisconnectAll();
	m_nameToNodeIndex.erase(it);
	m_nodes.erase(m_nodes.begin() + idx);
	for(auto &[name, idxOther] : m_nameToNodeIndex) {
		if(idxOther > idx) {
			--idxOther;
			m_nodes.at(idxOther)->nodeIndex = idxOther;
		}
	}
	return true;
}
void Graph::AddNode(const std::shared_ptr<GraphNode> &node)
{
	std::string name {(*node)->GetType()};
	size_t i = 1;
	for(;;) {
		auto namei = name + std::to_string(i);
		if(m_nameToNodeIndex.find(namei) == m_nameToNodeIndex.end()) {
			name = std::move(namei);
			break;
		}
		++i;
	}

	node->SetName(name);
	m_nodes.push_back(node);
	m_nameToNodeIndex[name] = m_nodes.size() - 1;
}
std::shared_ptr<GraphNode> Graph::AddNode(const std::string &type)
{
	auto node = m_nodeRegistry->GetNode(type);
	if(!node)
		return nullptr;
	auto inst = std::make_shared<GraphNode>(*this, *node);
	AddNode(inst);
	return inst;
}

std::vector<GraphNode *> Graph::TopologicalSort(const std::vector<std::shared_ptr<GraphNode>> &nodes) const
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
	auto sortedNodes = TopologicalSort(m_nodes);
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

bool Graph::Load(const std::string &filePath, std::string &outErr)
{
	std::shared_ptr<udm::Data> data {};
	try {
		data = udm::Data::Load(filePath);
	}
	catch(const udm::Exception &e) {
		outErr = e.what();
		return false;
	}
	if(!data) {
		outErr = "Failed to load shader graph data!";
		return false;
	}
	auto assetData = data->GetAssetData();

	auto udmData = assetData.GetData();
	auto result = Load(udmData, outErr);
	if(!result)
		return false;
	return true;
}

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
		auto inst = std::make_shared<GraphNode>(*this, *node);
		inst->SetName(name);
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

bool Graph::Save(const std::string &filePath, std::string &outErr) const
{
	auto data = udm::Data::Create();
	auto assetData = data->GetAssetData();
	auto result = Save(assetData, outErr);
	if(!result)
		return false;
	try {
		result = data->SaveAscii(filePath);
	}
	catch(const udm::Exception &e) {
		outErr = e.what();
		return false;
	}
	if(!result) {
		outErr = "Failed to save shader graph data!";
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

void Graph::DoGenerateGlsl(std::ostream &outHeader, std::ostream &outBody, const std::optional<std::string> &namePrefix)
{
	// We can't use an iterator here because we need to expand nodes, which may add new nodes during iteration
	size_t i = 0;
	while(i < m_nodes.size()) {
		auto &node = m_nodes[i];
		node->node.Expand(*this, *node);
		++i;
	}

	auto sortedNodes = TopologicalSort(m_nodes);

	uint32_t nodeIndex = 0;
	for(auto &node : sortedNodes)
		node->nodeIndex = nodeIndex++;

	std::unordered_set<std::string> requiredModules;
	for(const auto &node : sortedNodes) {
		for(const auto &dep : node->node.GetModuleDependencies())
			requiredModules.insert(dep);
	}

	for(const auto &dep : requiredModules) {
		outHeader << "// " << dep << "\n";
		outHeader << "#include \"/modules/" << dep << ".glsl\"\n";
	}

	for(const auto &node : sortedNodes) {
		outHeader << "// " << node->GetName() << " (" << (*node)->GetType() << ")\n";
		outHeader << node->node.EvaluateResourceDeclarations(*this, *node);
		outHeader << "\n";
	}

	// Traverse nodes and generate GLSL code for each
	for(const auto &node : sortedNodes) {
		outBody << "// " << node->GetName() << " (" << (*node)->GetType() << ")\n";
		outBody << node->node.Evaluate(*this, *node);
		outBody << "\n";
	}
}

void Graph::GenerateGlsl(std::ostream &outHeader, std::ostream &outBody, const std::optional<std::string> &namePrefix) const
{
	// To generate the GLSL code we need to expand all nodes (such as group nodes),
	// which modifies the graph. We create a copy so we don't have to modify the original graph.
	auto cpy = *this;
	cpy.DoGenerateGlsl(outHeader, outBody, namePrefix);
}
