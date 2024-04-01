#pragma once
#include <vector>

namespace AI
{
	enum class Status
	{
		Running,
		Success,
		Failure,
		Invalid
	};

	class TreeNode
	{
	public:

		virtual ~TreeNode() {}
		virtual Status Update() = 0;
		virtual bool Awake() = 0;
		virtual void Init() { __noop; };
		//virtual void Reset() { __noop; };
		virtual void Reset() 
		{ 
			//myStatus = Status::Invalid; 
		};

		Status Tick()
		{
			if (myStatus != Status::Running) {
				Init();
			}

			myStatus = Update();

			if (myStatus != Status::Running) {
				Reset();
			}

			return myStatus;
		}

	protected:
		Status myStatus = Status::Invalid;
	};

	class Composite : public TreeNode
	{
	public:
		Composite() : it(children.begin()), TreeNode()
		{
			if (children.size() > 0)
			{
				it = children.begin();
			}

		}
		virtual ~Composite() {}

		TreeNode* AddChild(TreeNode* aNode)
		{
			children.push_back(aNode);

			return aNode;
		}
		virtual void Init() override
		{
			it = children.begin();
		}
		virtual bool Awake() override
		{
			for (auto& child : children)
			{
				if (!child->Awake()){
					KE_LOG("Composite: Awake failed");
					return false;
				}
			}
			return true;
		}
		void Reset() override
		{
			for (auto& child : children)
			{
				child->Reset();
			}
		}
		virtual Status Update() = 0;

	protected:
		std::vector<TreeNode*> children;
		std::vector<TreeNode*>::iterator it;
	};

	class Decorator : public TreeNode
	{
	public:
		virtual ~Decorator() {}

		virtual bool Awake() = 0;
		virtual void Init()
		{

		}

		TreeNode* SetChild(TreeNode* aNode)
		{
			child = aNode;

			return child;
		}

	protected:
		TreeNode* child = nullptr;
	};

	class Leaf : public TreeNode
	{
	public:
		Leaf() {};
		virtual ~Leaf() {}
		virtual Status Update() = 0;
		virtual bool Awake() = 0;
		virtual void Reset() { __noop; };
	protected:
	};

	class Selector : public Composite
	{
	public:
		Selector() : Composite() {}

		Status Update() override
		{
			assert(children.size() && "Composite has no children");

			while (it != children.end())
			{
				Status status = (*it)->Tick();

				if (status != Status::Failure) {
					return status;
				}

				it++;
			}

			return Status::Failure;
		}
	};

	class Sequence : public Composite
	{
	public:
		Sequence() : Composite()
		{
			Init();
		}

		Status Update() override
		{
			assert(children.size() && "Sequence has no children");

			while (it != children.end())
			{
				Status status = (*it)->Tick();

				if (status != Status::Success) {
					return status;
				}

				it++;
			}

			return Status::Success;
		}
	};

} // namespace AI