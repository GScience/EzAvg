local p = {}

if sprite.type ~= "group" then
	logError("�޷���������Ϊ�ű��󶨵��Ǿ����������")
end

-- �Ƿ񼤻�
p.enabled = true
-- �Ƿ�����
p.destroyed = false
-- ���
p.space=10
-- ��������
p.layout=1

sprite.autoLayout = false

-- ��Ϊˢ��
function p.update()

	-- ���տ������
	local totalWidth = 0;
	for k,v in sprite.iter do
		if v.size[1] ~= 0 then
			local spriteBox = v.box
			spriteBox[1] = sprite.rect[1] + totalWidth
			spriteBox[2]  = sprite.rect[2]
			spriteBox[3]  = v.size[1]
			spriteBox[4]  = sprite.rect[4]
			v.box = spriteBox
			totalWidth = v.size[1] + totalWidth + p.space
		else
			logError("ˮƽ���ֶ���Ӧ���ÿ��")
		end
	end

	if p.layout == 1 then
		local offset = sprite.rect[3] / 2 - totalWidth / 2
		for k,v in sprite.iter do
			local spriteBox = v.box
			spriteBox[1] =  spriteBox[1] + offset
			v.box = spriteBox
		end
	elseif p.layout == 2 then
		local offset = sprite.rect[3] - totalWidth
		for k,v in sprite.iter do
			local spriteBox = v.box
			spriteBox[1] =  spriteBox[1] + offset
			v.box = spriteBox
		end
	end
end

-- ��Ϊ�Ƿ����
function p.isFinished()
	return false
end

return p