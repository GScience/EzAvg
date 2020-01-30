local p = {}

function p.start(args)
	if args.name == nil then
		logError("Î´Ö¸¶¨name")
		return
	end
	Sprite.setProperties(args.name, args)
end

return p