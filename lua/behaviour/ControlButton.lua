local p = {}

p.enabled = true
p.colorOnClick = {0.5,0.5,0.5}
p._isClicked = false
p._clickedFrameCount = 0

function p.update()
	if p._clickedFrameCount ~= Time.frameCount() then
		p._isClicked=false
	end
end

function p.onMouseDown()
	sprite.color = p.colorOnClick
end

function p.onMouseUp()
	sprite.color = {1,1,1}
	p._isClicked = true
	p._clickedFrameCount = Time.frameCount()
end

function p.onMouseLeave()
	sprite.color = {1,1,1}
end

return p