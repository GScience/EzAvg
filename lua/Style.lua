
require("StyleSheets")

function applyStyle(sprite, style)

	if type(sprite) ~= "table" then
		logError("精灵参数非法，无法应用样式")
	end

	if style == nil then
		logError("指定样式为空，无法应用样式")
	end

	for k,v in pairs(style) do
		if type(v) == "table" and v.type ~= nil then -- 如果有type属性，则是精灵
			local subSpriteType = v.type

			if sprite.type ~= "group" then
				logError("尝试向非精灵组对象中插入精灵")
			end
			local subSprite = sprite.addSprite(k, subSpriteType)
			applyStyle(subSprite, v)
		elseif type(v) == "table" and k == "behaviours" then-- 如果是behaviours数组，则是行为脚本

			for behaviourName,behaviourInfo in pairs(v) do

				if behaviourInfo.type == nil then
					logError("无法应用样式，因为行为脚本" .. behaviourName .. "的类型为空")
				end

				local behaviour = sprite.addBehaviour(behaviourName, behaviourInfo.type)
				for argName,argValue in pairs(behaviourInfo) do
					if argName ~= "type" then
						behaviour[argName] = argValue
					end
				end
			end
		else -- 如果都不是，直接设置属性
			if k ~= "type" then
				sprite[k] = v
			end
		end
	end
end
