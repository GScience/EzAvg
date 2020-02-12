local p = {}

p.enabled = true
p.time = 1
p.destMargin = {1,0,0,0}

p._lifeTime = 0
p._srcMargin = nil

function p.update()

	if p._srcMargin == nil then
		p._srcMargin = sprite.margin
	end

	p._lifeTime = p._lifeTime + Time.deltaTime()

	if p._lifeTime > p.time then

		for i = 1, 4, 1 do
			sprite.margin[i] = p.destMargin[i]
		end

		p.enabled = false

	else

		local currentMargin = {}

		for i = 1, 4, 1 do
			currentMargin[i] = p._srcMargin[i] + (p.destMargin[i] - p._srcMargin[i]) * p._lifeTime / p.time
		end
		sprite.margin = currentMargin;

	end
end

function p.isFinished()

	return p._lifeTime > p.time
end

return p