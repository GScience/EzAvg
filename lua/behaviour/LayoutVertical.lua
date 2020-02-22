local p = {}

if sprite.type ~= "group" then
	logError("无法将布局行为脚本绑定到非精灵组对象上")
end

-- 是否激活
p.enabled = true
-- 是否被销毁
p.destroyed = false
-- 间距
p.space=10
-- 布局类型
p.verticalLayout="center"
p.horizontalLayout="center"

sprite.autoLayout = false

-- 行为刷新
function p.update()

	-- 按照宽度排列
	local totalHeight = 0;
	for k,v in sprite.iter do
		if v.size[2] ~= 0 then
			local spriteBox = v.box
			
			if (p.horizontalLayout == "left") then
				spriteBox[1] = sprite.rect[1]
			elseif (p.horizontalLayout == "center") then
				spriteBox[1] = sprite.rect[1] + sprite.rect[3] / 2 - v.size[1] / 2 
			elseif (p.horizontalLayout == "right") then
				spriteBox[1] = sprite.rect[1] + sprite.rect[3] - v.size[1]
			end

			spriteBox[2] = sprite.rect[2] + totalHeight
			spriteBox[3] = sprite.rect[3]
			spriteBox[4] = v.size[1]
			v.box = spriteBox
			totalHeight = v.size[2] + totalHeight + p.space
		else
			logError("垂直布局对象应设置高度")
		end
	end

	if p.verticalLayout == "center" then
		local offset = sprite.rect[4] / 2 - totalHeight / 2
		for k,v in sprite.iter do
			local spriteBox = v.box
			spriteBox[2] =  spriteBox[2] + offset
			v.box = spriteBox
		end
	elseif p.verticalLayout == "bottom" then
		local offset = sprite.rect[4] - totalHeight
		for k,v in sprite.iter do
			local spriteBox = v.box
			spriteBox[2] =  spriteBox[2] + offset
			v.box = spriteBox
		end
	end
end

-- 行为是否结束
function p.isFinished()
	return false
end

return p