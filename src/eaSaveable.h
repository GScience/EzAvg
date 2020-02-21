#pragma once
#include "eaProfileNode.h"

class eaSaveable
{
public:
	virtual void Save(eaProfileNode& node) = 0;
	virtual void Load(eaProfileNode& node) = 0;
};