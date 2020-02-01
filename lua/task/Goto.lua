local p = {}

function p.start(args)

	if args.scene == nil and args.pos == nil then
		logError("未指定pos或scene")
		return
	end

	if args.scene ~= nil then
		Application.loadScene(args.scene)
	end

	if args.pos ~= nil then
		Script.jump(args.pos)
	end
end

return p