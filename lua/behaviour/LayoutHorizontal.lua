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
	local totalWidth = 0;
	for k,v in sprite.iter do
		if v.size[1] ~= 0 then
			local spriteBox = v.box
			spriteBox[1] = sprite.rect[1] + totalWidth

			if (p.verticalLayout == "top") then
				spriteBox[2]  = sprite.rect[2]
			elseif (p.verticalLayout == "center") then
				spriteBox[2]  = sprite.rect[2] + sprite.rect[4] / 2 - v.size[2] / 2 
			elseif (p.verticalLayout == "bottom") then
				spriteBox[2]  = sprite.rect[2] + sprite.rect[4] - v.size[2]
			end

			spriteBox[3]  = v.size[1]
			spriteBox[4]  = sprite.rect[4]
			v.box = spriteBox
			totalWidth = v.size[1] + totalWidth + p.space
		else
			logError("水平布局对象应设置宽度")
		end
	end

	if p.horizontalLayout == "center" then
		local offset = sprite.rect[3] / 2 - totalWidth / 2
		for k,v in sprite.iter do
			local spriteBox = v.box
			spriteBox[1] =  spriteBox[1] + offset
			v.box = spriteBox
		end
	elseif p.horizontalLayout== "right" then
		local offset = sprite.rect[3] - totalWidth
		for k,v in sprite.iter do
			local spriteBox = v.box
			spriteBox[1] =  spriteBox[1] + offset
			v.box = spriteBox
		end
	end
end

-- 行为是否结束
function p.isFinished()
	return false
end

return p