local p = {}

function p.start(args)
	if args.pos == nil then
		logError("未指定pos")
		return
	end
	Script.jump(args.pos)
end

return p