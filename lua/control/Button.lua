require("StyleSheets")

local p = {}

p.parent = nil
p.text = ""
p.background = ""

function p:Create(name)

	if self == nil then
		LogError("Control create should call by : but not .")
	end

	if self.parent == nil then
		self.parent = scene
	end

	if name == nil then
		LogError("Name cant be null")
	end

	-- 创建按钮精灵组
	local controlGroup = self.parent.addSprite(name, "group")
	ApplyStyle(controlGroup, Button)

	controlGroup.Text.text = self.text
	controlGroup.Image.image = self.background

	return controlGroup
end

return p