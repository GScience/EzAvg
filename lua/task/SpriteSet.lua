local p = {}

function p.start(args)

	local sprite

	if args.sprite == nil then
		sprite = scene
	else
		sprite = scene[args.sprite]
	end

	if sprite == nil then
		logError("未找到精灵")
		return
	end

	for k,v in pairs(args) do
		sprite[k] = v
	end
end

return p