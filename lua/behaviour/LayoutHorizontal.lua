local p = {}

-- �Ƿ񼤻�
p.enabled = true
-- �Ƿ�����
p.destroyed = false

-- ��Ϊˢ��
function p.update()
	
end

-- ��Ϊ��ʼ
function p.start()
	print("start")
	for k,v in sprite.iter do
		print(v.name)
	end
end

-- ��Ϊ�Ƿ����
function p.isFinished()
	return false
end

return p