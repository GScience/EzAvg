local p = {}

function p.start(args)
	if args.name == nil then
		logError("δָ��name")
		return
	end
	Sprite.setProperties(args.name, args)
end

return p