local p = {}

function p.start(args)
	if args.name == nil then
		logError("δָ��name")
		return
	end
	if args.type == nil then
		logError("δָ��type")
		return
	end
	if not Sprite.create(args.name, args.type) then
		logError("����" .. args.name .. "�Ѵ���")
		return
	end
end

return p