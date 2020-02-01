#pragma once
#include <string>
#include <memory>
#include "eaSaveable.h"

class eaLuaDomain;

class eaLuaBridge : public eaSaveable
{
protected:
	int objRef;
	const std::string name;
	std::shared_ptr<eaLuaDomain> domain;
	eaLuaBridge(std::shared_ptr<eaLuaDomain> owner, std::string name);

public:
	virtual ~eaLuaBridge();
	void Dispose();

	void Save() override;
	void Load() override;
};