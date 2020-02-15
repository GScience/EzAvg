
function ApplyStyle(sprite, style)
	for k,v in pairs(style) do
		if type(v) == "table" and v.type ~= nil then -- 如果是table，则创建精灵
			local subSpriteType = v.type

			if subSpriteType == nil then
				subSpriteType = "group"
			end

			local subSprite = sprite.addSprite(k, subSpriteType)
			ApplyStyle(subSprite, v)
		else -- 如果不是table，直接设置属性
			if k ~= "type" then
				sprite[k] = v
			end
		end
	end
end

Button = 
{
	Image = 
	{
		type = "image",
		zOrder = 0
	},
	Text = 
	{
		type = "text",
		verticalLayout = "center",
		horizontalLayout = "center",
		color = {0.8,0.8,0.8},
		fontSize = 36,
		zOrder = 1
	},
	propertyTable=
	{
		text="Text.text"
	}
}