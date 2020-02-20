local p = {}

p.enabled = true
p.colorOnClick = {0.5,0.5,0.5}
p.colorOnEnter = {1,1,1}
p.colorNormal = {0.8,0.8,0.8}
p._isClicked = false
p._clickedFrameCount = 0
p._init = false

function p.update()
	
	if p._init == false then
		sprite.color = p.colorNormal
		p._init = true
	end

	if p._clickedFrameCount ~= Time.frameCount() then
		p._isClicked=false
	end
end

function p.onMouseDown()
	sprite.color = p.colorOnClick
end

function p.onMouseUp()
	sprite.color = p.colorNormal
	p._isClicked = true
	p._clickedFrameCount = Time.frameCount()
end

function p.onMouseLeave()
	sprite.color = p.colorNormal
end

function p.onMouseEnter()
	sprite.color = p.colorOnEnter
end

return p