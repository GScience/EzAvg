local p = {}

function p.start(args)
	if args.name == nil then
		logError("δָ��name")
		return
	end
	Sprite.remove(args.name)
end

return p