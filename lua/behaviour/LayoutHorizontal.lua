local p = {}

-- 是否激活
p.enabled = true
-- 是否被销毁
p.destroyed = false

-- 行为刷新
function p.update()
	
end

-- 行为是否结束
function p.isFinished()
	return false
end

return p