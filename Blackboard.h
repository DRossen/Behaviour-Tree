#pragma once
#include <any>

namespace AI
{
	class Blackboard
	{
	public:

		template <typename T>
		inline void Add(const std::string& aKey, const T& aValue)
		{
			data[aKey] = aValue;
		}

		// Returns nullptr if the value does not exist.
		template <typename T>
		inline T* Get(const std::string& aKey) 
		{
			if (data.find(aKey) == data.end()) {
				return nullptr;
			}

			return &std::any_cast<T&>(data[aKey]);
		}

		// Returns wether the value exists or not.
		inline bool Has(const std::string& aKey)
		{
			return data.find(aKey) != data.end();
		}
		 
	private:
		std::unordered_map<std::string, std::any> data;
		int invalidInt = INT_MIN;
		
	};
}