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

import :node;

using namespace pragma::shadergraph;

Node::Node(const std::string_view &type) : m_type {type} {}
Node::~Node() {}

const std::string_view &Node::GetType() const { return m_type; }
std::string Node::Evaluate(const Graph &graph, const NodeInstance &instance) const
{
	auto res = DoEvaluate(graph, instance);
	//m_curVarId = 0;
	return res;
}
void Node::AddOutput(const std::string &name, SocketType type) { m_outputs.emplace_back(name, type); }

std::optional<size_t> Node::FindOutputIndex(const std::string &name) const
{
	auto it = std::find_if(m_outputs.begin(), m_outputs.end(), [&name](const Socket &socket) { return socket.name == name; });
	return (it != m_outputs.end()) ? (it - m_outputs.begin()) : std::optional<size_t> {};
}
std::optional<size_t> Node::FindInputIndex(const std::string &name) const
{
	auto it = std::find_if(m_inputs.begin(), m_inputs.end(), [&name](const Socket &socket) { return socket.name == name; });
	return (it != m_inputs.end()) ? (it - m_inputs.begin()) : std::optional<size_t> {};
}
const std::vector<Socket> &Node::GetInputs() const { return m_inputs; }
const std::vector<Socket> &Node::GetOutputs() const { return m_outputs; }
const Socket *Node::GetInput(size_t index) const { return (index < m_inputs.size()) ? &m_inputs[index] : nullptr; }
const Socket *Node::GetOutput(size_t index) const { return (index < m_outputs.size()) ? &m_outputs[index] : nullptr; }

std::string Node::GetInputNameOrValue(const NodeInstance &instance, uint32_t inputIdx) const
{
	std::string val;
	auto &input = instance.inputs.at(inputIdx);
	if(input.link && input.link->parent)
		val = input.link->parent->GetOutputVarName(input.link->outputIndex);
	else
		input.GetValue(val);
	return val;
}
std::string Node::GetInputNameOrValue(const NodeInstance &instance, const std::string_view &inputName) const
{
	auto it = std::find_if(m_inputs.begin(), m_inputs.end(), [&inputName](const Socket &socket) { return socket.name == inputName; });
	if(it == m_inputs.end())
		throw std::invalid_argument {"No input named '" + std::string {inputName} + "' exists!"};
	return GetInputNameOrValue(instance, it - m_inputs.begin());
}
