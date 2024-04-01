#pragma once
#include "TreeNode.h"
#include "Blackboard.h"

namespace AI
{
	class BehaviourTree : public TreeNode
	{
		friend class BehaviourTreeBuilder;

	public:
		BehaviourTree() : blackboard(new Blackboard()) {}
		~BehaviourTree() { delete blackboard; }

		inline bool Awake() override { return root->Awake(); }
		inline Status Update() override { return root->Tick(); }
		void SetRoot(TreeNode* aRoot) { root = aRoot; }
		inline void Reset() override { root->Reset(); }

		Blackboard* blackboard = nullptr;
		TreeNode* root = nullptr;
	private:
	};

}