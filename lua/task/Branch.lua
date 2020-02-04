local p = {}

p.finished = false

function p.start(args)

	if args.name == nil then
		logError("未指定name数组")
		return
	end

	p.branchName = args.name

	if args.pos == nil then
		logError("未指定pos数组")
		return
	end

	p.pos=args.pos

	if args.offset == nil then
		logError("未指定offset")
		return
	end

	p.offset = args.offset

	if args.size == nil then
		logError("未指定size")
		return
	end

	if args.image == nil then
		logError("未指定image")
		return
	end

	for k,v in pairs(p.branchName) do
		local spriteName = "_branch" .. v
		local sprite = Sprite.create(spriteName, "image")
		sprite.anchor = {0.5,0.5,0.5,0.5}
		local newPosX =  sprite.position[1] + p.offset[1]
		local newPosY =  sprite.position[2] + p.offset[2]
		local position =  { newPosX, newPosY }
		print(position)
		sprite.position = position
		sprite.size = p.size
		sprite.image = args.image
		local behaviour = Sprite.addBehaviour(spriteName, "button", "controlButton")
	end
end

function p.update()

end

return p