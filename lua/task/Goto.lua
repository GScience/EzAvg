local p = {}

function p.start(args)
	if args.pos == nil then
		logError("Î´Ö¸¶¨pos")
		return
	end
	ActiveScene.scriptJump(args.pos)
end

return p