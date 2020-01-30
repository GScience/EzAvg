local p = {}

function p.start(args)
	if args.name == nil then
		logError("未指定name")
		return
	end
	if args.type == nil then
		logError("未指定type")
		return
	end
	if not Sprite.create(args.name, args.type) then
		logError("精灵" .. args.name .. "已存在")
		return
	end
end

return p