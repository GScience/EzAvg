local p = {}

function p.start(args)
	if args.name == nil then
		logError("未指定name")
		return
	end
	if args.behaviour == nil then
		logError("未指定behaviour")
		return
	end

	Sprite.addBehaviour(args.name, args.behaviour, args)
end

return p