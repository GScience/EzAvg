#pragma once
#include "eaProfileNode.h"

class eaSaveable
{
public:
	virtual void Save(std::shared_ptr<eaProfileNode> node) = 0;
	virtual void Load(std::shared_ptr<eaProfileNode> node) = 0;
};