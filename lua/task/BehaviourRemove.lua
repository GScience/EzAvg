local p = {}

function p.start(args)

	if args.sprite == nil then
		logError("未指定sprite")
		return
	end

	if args.behaviour == nil then
		logError("未指定behaviour")
		return
	end

	local sprite = scene[args.sprite]
	if sprite == nil then
		logError("未找到精灵对象")
		return
	end

	local result = sprite[args.behaviour]
	if result == nil then
		logError("未找到行为对象")
		return
	end

	result.destroyed = true
end

return p