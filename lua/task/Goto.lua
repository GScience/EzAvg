local p = {}

function p.start(args)
	if args.pos == nil then
		logError("未指定pos")
		return
	end
	ActiveScene.scriptJump(args.pos)
end

return p