local p = {}

p.enabled = true
p.speed = 0.1

function p.update()
	local alpha = sprite.alpha
	alpha = alpha + Time.deltaTime() * p.speed
	if alpha < 0 then
		alpha = 0
		p.enabled = false
	elseif alpha > 1 then
		alpha = 1
		p.enabled = false
	end
	sprite.alpha = alpha
end

function p.isFinished()

	if p.speed > 0 then
		return sprite.alpha >= 1
	elseif p.speed < 0 then
		return sprite.alpha <= 0
	else
		return true
	end
end

return p