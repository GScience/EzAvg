local p = {}

function p.start(args)

	if args.name == nil then
		logError("无法创建行为，未指定name")
		return
	end

	if args.behaviour == nil then
		logError("无法创建行为，未指定behaviour")
		return
	end
	
	if args.type == nil then
		ogError("无法创建行为，未指定type")
		return
	end

	local sprite = scene[args.name]
	if sprite == nil then
		logError("无法创建行为，未找到精灵对象")
		return
	end

	if sprite[args.behaviour] ~= nil then
		logError("无法创建行为，行为脚本或属性" .. args.behaviour .. "已存在")
		return
	end

	local result = Sprite.addBehaviour(args.name, args.behaviour, args.type, args)
	for k,v in pairs(args) do
		if k ~= "name" and k ~= "behaviour" and k ~= "type" then
			result[k] = v
			print(k.." = "..v)
		end
	end
end

return p