/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string>
#include <string_view>
#include <stdexcept>

module pragma.shadergraph;

import :node_registry;

using namespace pragma::shadergraph;

NodeRegistry::NodeRegistry() {}
const std::shared_ptr<Node> NodeRegistry::GetNode(const std::string &name) const
{
	auto it = m_nodes.find(name);
	if(it == m_nodes.end())
		return nullptr;
	return it->second;
}
void NodeRegistry::RegisterNode(const std::string &name, const std::shared_ptr<Node> &node) { m_nodes[name] = node; }
