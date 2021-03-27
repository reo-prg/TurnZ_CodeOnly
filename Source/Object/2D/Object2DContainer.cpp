#include "Object2DContainer.h"
#include <cassert>
#include <Windows.h>
#include "Object2D.h"

void Object2DContainer::Add(Object2D* object, const std::string& key)
{
	if (object_.find(key) != object_.end()) { return; }
	object_.emplace(key, object);
}

const std::shared_ptr<Object2D>& Object2DContainer::Get(const std::string& key)
{
	assert(object_.find(key) != object_.end());
	return object_.at(key);
}
