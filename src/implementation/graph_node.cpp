/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Copyright (c) 2024 Silverlan
*/

module;

#include <string>
#include <cassert>
#include <stdexcept>

module pragma.shadergraph;

import :graph_node;

using namespace pragma::shadergraph;

OutputTest::OutputTest(NodeInstance &node, uint32_t index) : parent {&node}, outputIndex {index} {}
const Socket &OutputTest::GetSocket() const { return *parent->node.GetOutput(outputIndex); }

InputTest::InputTest(NodeInstance &node, uint32_t index) : parent {&node}, inputIndex {index}, value {GetSocket().type} {}
InputTest::~InputTest() { ClearValue(); }
void InputTest::ClearValue() { value.Clear(); }
const Socket &InputTest::GetSocket() const { return *parent->node.GetInput(inputIndex); }

NodeInstance::NodeInstance(Node &node, const std::string &name) : node {node}, m_name {name}
{
	auto &nodeInputs = node.GetInputs();
	inputs.reserve(nodeInputs.size());
	for(uint32_t i = 0; i < nodeInputs.size(); ++i)
		inputs.emplace_back(*this, i);

	auto &nodeOutputs = node.GetOutputs();
	outputs.reserve(nodeOutputs.size());
	for(uint32_t i = 0; i < nodeOutputs.size(); ++i)
		outputs.emplace_back(*this, i);
}
std::string NodeInstance::GetName() const { return m_name; }
void NodeInstance::ClearInputValue(const char *inputName)
{
	auto inputIdx = node.FindInputIndex(inputName);
	if(!inputIdx)
		return;
	auto &input = inputs[*inputIdx];
	input.ClearValue();
}
bool NodeInstance::Disconnect(const char *inputName)
{
	auto inputIdx = node.FindInputIndex(inputName);
	if(!inputIdx)
		return false;
	auto &input = inputs[*inputIdx];
	if(!input.link)
		return false;
	auto it = std::find(input.link->links.begin(), input.link->links.end(), &input);
	assert(it != input.link->links.end());
	input.link->links.erase(it);
	input.link = nullptr;
	return true;
}
bool NodeInstance::Disconnect(const char *outputName, NodeInstance &linkTarget, const char *inputName)
{
	auto outputIdx = node.FindOutputIndex(outputName);
	if(!outputIdx)
		return false;
	auto inputIdx = linkTarget.node.FindInputIndex(inputName);
	if(!inputIdx)
		return false;
	auto &output = outputs.at(*outputIdx);
	auto it = std::find_if(output.links.begin(), output.links.end(), [&linkTarget, &inputIdx](const InputTest *input) { return input->parent == &linkTarget && input->inputIndex == *inputIdx; });
	if(it == output.links.end())
		return false;
	auto &link = *it;
	link->parent->Disconnect(inputName);
	return true;
}
bool NodeInstance::CanLink(const char *outputName, NodeInstance &linkTarget, const char *inputName) const
{
	if(&linkTarget == this)
		return false;
	// TODO: Check if types are compatible
	return true;
}
bool NodeInstance::Link(const char *outputName, NodeInstance &linkTarget, const char *inputName)
{
	if(!CanLink(outputName, linkTarget, inputName))
		return false;
	auto outputIdx = node.FindOutputIndex(outputName);
	if(!outputIdx)
		return false;
	auto inputIdx = linkTarget.node.FindInputIndex(inputName);
	if(!inputIdx)
		return false;
	Disconnect(outputName, linkTarget, inputName);

	auto &input = linkTarget.inputs[*inputIdx];
	auto &output = outputs[*outputIdx];
	output.links.push_back(&input);

	input.link = &output;
	return true;
}

std::string NodeInstance::GetOutputVarName(size_t outputIdx) const { return "var" + std::to_string(nodeIndex) + "_" + std::to_string(outputIdx); }
std::string NodeInstance::GetOutputVarName(const std::string_view &name) const
{
	auto it = std::find_if(outputs.begin(), outputs.end(), [&name](const OutputTest &output) { return output.GetSocket().name == name; });
	if(it == outputs.end())
		throw std::invalid_argument {"No output named '" + std::string {name} + "' exists!"};
	return GetOutputVarName(it - outputs.begin());
}
