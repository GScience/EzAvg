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

	local sprite = scene[args.name]
	if sprite == nil then
		logError("δ�ҵ��������")
		return
	end

	local result = sprite[args.behaviour]
	if result == nil then
		logError("δ�ҵ���Ϊ����")
		return
	end

	result.enabled = false
end

return p