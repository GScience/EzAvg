-- 可拖动的控件
local p = {}

-- 是否激活
p.enabled = true
-- 是否被销毁
p.destroyed = false

-- 是否可以水平拖动
p.canDragHorizontal = true
-- 是否可以垂直拖动
p.canDragVertical = true

-- 是否可以拖动
p._isDraging = false

-- 行为刷新
function p.update()

	if p._isDraging == false then
		return
	end
	local deltaPos = Input.getMousePointDelta()
	local margin = sprite.margin

	-- x
	if p.canDragHorizontal then
		if sprite.size[1] == 0 then
			margin[2] = margin[2] - deltaPos[1]
			margin[4] = margin[4] + deltaPos[1]
		else
			local margin = sprite.margin
			if margin[2] ~= 0 then
				margin[2] = margin[2] - deltaPos[1]
			else
				margin[4] = margin[4] + deltaPos[1]
			end
		end
	end

	if p.canDragVertical then
		if sprite.size[2] == 0 then
			margin[1] = margin[1] + deltaPos[2]
			margin[3] = margin[3] - deltaPos[2]
		else
			local margin = sprite.margin
			if margin[3] ~= 0 then
				margin[3] = margin[3] - deltaPos[2]
			else
				margin[1] = margin[1] + deltaPos[2]
			end
		end
	end

	sprite.margin = margin;
end

-- 行为是否结束
function p.isFinished()
end

function p.onMouseLeave()
	p._isDraging = false
end

function p.onMouseDown()
	p._isDraging = true
end

function p.onMouseUp()
	p._isDraging = false
end

return p