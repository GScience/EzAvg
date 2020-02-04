local p = {}

function p.start(args)

	if args.sprite == nil then
		logError("未指定sprite")
		return
	end

	local sprite = scene[args.sprite]
	if sprite == nil then
		logError("未找到精灵对象")
		return
	end

	sprite.destroyed = true
end

return p