local p = {}

function p.start(args)

	if args.behaviour == nil then
		logError("无法创建行为，未指定behaviour")
		return
	end
	
	if args.type == nil then
		logError("无法创建行为，未指定type")
		return
	end

	local sprite
	
	if args.sprite == nil then
		sprite = scene
	else
		sprite = scene[args.sprite]
	end

	if sprite == nil then
		logError("无法创建行为，未找到精灵对象")
		return
	end

	if sprite[args.behaviour] ~= nil then
		logError("无法创建行为，行为脚本或属性" .. args.behaviour .. "已存在")
		return
	end

	local result = sprite.addBehaviour(args.behaviour, args.type)
	for k,v in pairs(args) do
		if k ~= "sprite" and k ~= "behaviour" and k ~= "type" then
			result[k] = v
		end
	end
end

return p