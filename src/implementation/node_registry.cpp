// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string>
#include <string_view>
#include <stdexcept>
#include <memory>
#include <vector>

module pragma.shadergraph;

import :node_registry;

using namespace pragma::shadergraph;

NodeRegistry::NodeRegistry() {}
const std::shared_ptr<Node> NodeRegistry::GetNode(const std::string &name) const
{
	auto it = m_nodes.find(name);
	if(it == m_nodes.end()) {
		for(auto &child : m_childRegistries) {
			auto node = child->GetNode(name);
			if(node)
				return node;
		}
		return nullptr;
	}
	return it->second;
}
void NodeRegistry::GetNodeTypes(std::vector<std::string> &outNames) const
{
	outNames.reserve(outNames.size() + m_nodes.size());
	for(auto &node : m_nodes)
		outNames.push_back(node.first);
	for(auto &child : m_childRegistries)
		child->GetNodeTypes(outNames);
}
void NodeRegistry::RegisterNode(const std::string &name, const std::shared_ptr<Node> &node) { m_nodes[name] = node; }
