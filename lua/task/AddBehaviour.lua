local p = {}

function p.start(args)
	if args.name == nil then
		logError("δָ��name")
		return
	end
	if args.behaviour == nil then
		logError("δָ��behaviour")
		return
	end

	Sprite.addBehaviour(args.name, args.behaviour, args)
end

return p