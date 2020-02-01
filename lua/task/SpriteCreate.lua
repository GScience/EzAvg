local p = {}

function p.start(args)

	if args.name == nil then
		logError("无法创建精灵，未指定name")
		return
	end
	if args.type == nil then
		logError("无法创建精灵，未指定type")
		return
	end
	if scene == nil then
		logError("无法创建精灵，未找到场景")
		return
	end
	if scene[args.name] ~= nil then
		logError("无法创建精灵，精灵或属性" .. args.name .. "已存在")
		return
	end
	if not Sprite.create(args.name, args.type) then
		logError("无法创建精灵，精灵" .. args.name .. "创建失败")
		return
	end
end

return p