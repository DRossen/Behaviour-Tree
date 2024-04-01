#pragma once
#include "Project/Source/Enemies/EnemyData.h"
#include "Engine/Source/AI/TreeNode.h"
#include <Engine/Source/Script/Script.h>

namespace AI
{
	class BehaviourTree;
}

namespace AI
{
	struct RecursiveData
	{
		RecursiveData(KE::Script* aScript) :
			script(aScript),
			scriptNodes(aScript->GetNodes()),
			connections(aScript->GetConnections()) {}

		KE::Script* script = nullptr;
		std::unordered_map<KE::ScriptMemberID, KE::ScriptNode*, KE::ScriptMemberID>& scriptNodes;
		std::unordered_map<KE::ScriptMemberID, std::vector<KE::PinConnection>, KE::ScriptMemberID>& connections;
	};

	enum class TreeNodeType {
		Sequence,
		Selector,
		StateSelector,
		FLOW_END,

		LEAF_START,
		Attack,
		ChasePlayer,
		RunToGoal,
		SwitchState,
		LEAF_END,

		DECORATOR_START,
		WithinAttackRange,
		ShouldGoCombat,
		InsideCombatArea,
		DECORATOR_END,

		COUNT,
	};


	inline std::string typeNames[(int)TreeNodeType::COUNT] = {
		"Sequence",
		"Selector",
		"StateSelector",
		"FLOW_END",

		"LEAF_START",
		"Attack",
		"ChasePlayer",
		"RunToGoal",
		"SwitchState",
		"LEAF_END",

		"DECORATOR_START",
		"WithinAttackRange",
		"ShouldGoCombat",
		"InsideCombatArea",
		"DECORATOR_END",
	};
}

namespace KE
{
	class Script;
	struct ScriptMemberID;

	using namespace AI;
	class BehaviourTreeBuilder
	{
	public:

		static AI::BehaviourTree* GenerateBT(const EnemyType aType);

	private:
		static bool RecursiveAttach(KE::ScriptMemberID& aFromNodeID, AI::TreeNode* aTreeNode, RecursiveData& aData);
		static TreeNodeType EvaluateEnum(KE::ScriptNode*);

		static ScriptNode* GetEditorRootNode(AI::RecursiveData& aData, std::string aRootName);
		static KE::Script* GetEnemyScript(const EnemyType aType);
		static TreeNode* GetTreeNode(TreeNodeType aType);
	};


}