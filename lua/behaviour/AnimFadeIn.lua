local p = {}

p.enabled = true
p.speed = 0.1

function p.update()
	local alpha = sprite.alpha
	alpha = alpha + Time.deltaTime() * p.speed
	if alpha < 0 then
		alpha = 0
	elseif alpha > 1 then
		alpha = 1
	end
	sprite.alpha = alpha
end

function p.start()
end

return p