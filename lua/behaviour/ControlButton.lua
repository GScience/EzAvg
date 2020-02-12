local p = {}

p.enabled = true

function p.update()
	if Input.getButton(0) then
		sprite.alpha = 0.5
	else
		sprite.alpha = 1
	end
end

return p