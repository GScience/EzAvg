local p = {}

function p.start(args)

	if args.name == nil then
		logError("未指定name")
		return
	end

	local sprite = scene[args.name]

	if sprite == nil then
		logError("未找到精灵")
		return
	end

	for k,v in pairs(args) do
		sprite[k] = v
	end
end

return p