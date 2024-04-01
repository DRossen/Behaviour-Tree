#include "stdafx.h"
#include "BehaviourTreeBuilder.h"
#include "BehaviourTree.h"

#include <Engine/Source/Script/ScriptManager.h>
#include <Project/Source/Enemies/Behaviours/LeafNodes.h>
#include <Project/Source/Enemies/Behaviours/DecoratorNodes.h>
#include <Project/Source/Enemies/Behaviours/SpecialComposite.h>

AI::BehaviourTree* tree = nullptr;

namespace KE
{
	AI::BehaviourTree* BehaviourTreeBuilder::GenerateBT(EnemyType aType)
	{
		KE::Script* script = GetEnemyScript(aType);

		if (!script) {
			KE_WARNING("BT-Builder did not find script.");
			return nullptr;
		}

		tree = nullptr;
		tree = new AI::BehaviourTree();

		RecursiveData data(script);

		KE::ScriptNode* scriptRoot = GetEditorRootNode(data, "Behaviour Root");

		if (!scriptRoot) {
			KE_WARNING("BT-Builder did not find root node.");
			return nullptr;
		}

		// The root node's output pin ID
		KE::ScriptMemberID rootOutPinID = scriptRoot->GetOutputPins()[0].ID;

		// Get PinConnections from root's outputpin.
		std::vector<KE::PinConnection>& rootConnections = data.connections[rootOutPinID];

		if (rootConnections.empty())
		{
			KE_WARNING("BT-Builder did not find any connections from root node.");
			return nullptr;
		}
		auto nodeID = data.connections[rootOutPinID][0].to.GetNodeID();

		// Set the root node of the tree.
		TreeNodeType type = EvaluateEnum(data.scriptNodes[nodeID]);
		TreeNode* root = GetTreeNode(type);
		tree->SetRoot(root);

		RecursiveAttach(nodeID, tree->root, data);

		return tree;
	}

	TreeNodeType BehaviourTreeBuilder::EvaluateEnum(KE::ScriptNode* aNode)
	{
		char* dataPointer = (char*)aNode->GetCustomData();
		const TreeNodeType type = *(TreeNodeType*)(dataPointer);

		return type;
	}

	KE::Script* BehaviourTreeBuilder::GetEnemyScript(const EnemyType aType)
	{
		auto* scriptManager = KE_GLOBAL::blackboard.Get<ScriptManager>("scriptManager");
		std::string path = {};

		switch (aType)
		{
		case EnemyType::Default:
			path = "Daniels_MEGA_script";
			break;
		}

		KE::Script* script = scriptManager->GetOrLoadScript(path);

		return script;
	}

	ScriptNode* BehaviourTreeBuilder::GetEditorRootNode(AI::RecursiveData& aData, std::string aRootName)
	{
		auto& scriptNodes = aData.script->GetNodes();

		KE::ScriptNode* root = nullptr;
		for (auto& node : scriptNodes)
		{
			if (strcmp(node.second->GetName(), aRootName.c_str()) == 0)
			{
				root = node.second;
				return root;
			}
		}

		return nullptr;
	}

	TreeNode* BehaviourTreeBuilder::GetTreeNode(TreeNodeType aType)
	{
		TreeNode* node = nullptr;

		switch (aType)
		{
			// Composite Nodes //
		case AI::TreeNodeType::Sequence:
			node = new AI::Sequence();
			break;
		case AI::TreeNodeType::Selector:
			node = new AI::Selector();
			break;
		case AI::TreeNodeType::StateSelector:
			node = new AI::StateSelector(tree->blackboard);
			break;

			// Leaf Nodes //
		case AI::TreeNodeType::Attack:
			node = new AI::Attack(tree->blackboard);
			break;
		case AI::TreeNodeType::RunToGoal:
			node = new AI::RunToGoal(tree->blackboard);
			break;
		case AI::TreeNodeType::ChasePlayer:
			node = new AI::ChasePlayer(tree->blackboard);
			break;
		case AI::TreeNodeType::SwitchState:
			node = new AI::SwitchState(tree->blackboard);
			break;

			// Decorator Nodes //
		case AI::TreeNodeType::WithinAttackRange:
			node = new AI::WithinAttackRange(tree->blackboard);
			break;
		case AI::TreeNodeType::ShouldGoCombat:
			node = new AI::ShouldGoCombat(tree->blackboard);
			break;
		case AI::TreeNodeType::InsideCombatArea:
			node = new AI::InsideCombatArea(tree->blackboard);
			break;


		default:
			KE_LOG("BT-Builder(GetNode) did not find node: ", typeNames[static_cast<int>(aType)].c_str());
			break;
		}
		return node;
	}

	bool BehaviourTreeBuilder::RecursiveAttach(KE::ScriptMemberID& aScriptNode, AI::TreeNode* aTreeNode, RecursiveData& aData)
	{
		if (auto sequence = dynamic_cast<AI::Sequence*>(aTreeNode))
		{
			const auto& fromNode = aData.scriptNodes.at(aScriptNode);

			// A Sequence can only have one output pin but multiple children.
			auto id = fromNode->GetOutputPins()[0].ID;
			const auto& pinConnections = aData.connections[id];

			for (auto& inputPin : pinConnections)
			{
				auto nodeID = inputPin.to.GetNodeID();
				TreeNodeType type = EvaluateEnum(aData.scriptNodes[nodeID]);

				if (AI::TreeNode* childNode = GetTreeNode(type))
				{
					sequence->AddChild(childNode);
					RecursiveAttach(nodeID, childNode, aData);
				}
			}

			// [TODO] Sort the Sequencer children based position in the script.
			// [TODO] Sort the Sequencer children based position in the script.
			// [TODO] Sort the Sequencer children based position in the script.

		}
		else if (auto selector = dynamic_cast<AI::Selector*>(aTreeNode))
		{
			const auto& fromNode = aData.scriptNodes.at(aScriptNode);

			// A Selector can have one output pin but multiple children.
			auto id = fromNode->GetOutputPins()[0].ID;
			const auto& pinConnections = aData.connections[id];

			for (auto& inputPin : pinConnections)
			{
				auto nodeID = inputPin.to.GetNodeID();

				TreeNodeType type = EvaluateEnum(aData.scriptNodes[nodeID]);

				if (AI::TreeNode* childNode = GetTreeNode(type))
				{
					selector->AddChild(childNode);
					RecursiveAttach(nodeID, childNode, aData);
				}
			}

			// [TODO] Sort the Selector children based position in the script.
			// [TODO] Sort the Selector children based position in the script.
			// [TODO] Sort the Selector children based position in the script.
		}
		else if (auto decorator = dynamic_cast<AI::Decorator*>(aTreeNode))
		{
			const auto& fromNode = aData.scriptNodes.at(aScriptNode);

			// A Decorator can only have one child.
			auto id = fromNode->GetOutputPins()[0].ID;

			//if (aData.connections[id].size() != 1)
			//{
			//	KE_WARNING("BT-Builder did not find a valid connection for Decorator.");
			//	return false;
			//}
			auto nodeID = aData.connections[id][0].to.GetNodeID();

			TreeNodeType type = EvaluateEnum(aData.scriptNodes[nodeID]);

			if (AI::TreeNode* childNode = GetTreeNode(type))
			{
				decorator->SetChild(childNode);
				RecursiveAttach(nodeID, childNode, aData);
			}
		}
		else
		{
			KE_WARNING("BT-Builder did not find a valid node type.");
			return false;
		}

		return false;
	}
}