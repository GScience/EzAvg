local p = {}

function p.start(args)

	if args.sprite == nil then
		logError("�޷�������Ϊ��δָ��sprite")
		return
	end

	if args.behaviour == nil then
		logError("�޷�������Ϊ��δָ��behaviour")
		return
	end
	
	if args.type == nil then
		ogError("�޷�������Ϊ��δָ��type")
		return
	end

	local sprite = scene[args.sprite]
	if sprite == nil then
		logError("�޷�������Ϊ��δ�ҵ��������")
		return
	end

	if sprite[args.behaviour] ~= nil then
		logError("�޷�������Ϊ����Ϊ�ű�������" .. args.behaviour .. "�Ѵ���")
		return
	end

	local result = Sprite.addBehaviour(args.sprite, args.behaviour, args.type, args)
	for k,v in pairs(args) do
		if k ~= "sprite" and k ~= "behaviour" and k ~= "type" then
			result[k] = v
		end
	end
end

return p