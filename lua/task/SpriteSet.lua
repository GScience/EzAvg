local p = {}

function p.start(args)

	if args.name == nil then
		logError("未指定name")
		return
	end

	Sprite.setProperties(args.name, args)
end

return p