require("StyleSheets")

local p = {}

p.parent = nil

function p:Create(name)

	if self == nil then
		LogError("Control create should call by : but not .")
	end

	if p.parent == nil then
		LogError("Parent not found")
	end

	if name == nil then
		LogError("Name cant be null")
	end

	local controlGroup = p.parent.addSprite(name, "group")


end

return p