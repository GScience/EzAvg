local p = {}

p.enabled = true
p.onClickAlpha = 0.5

function p.update()
	if Input.getButton(0) then
		sprite.transparent = p.onClickAlpha
	else
		sprite.transparent = 1
	end
end

return p