local p = {}

p.enabled = true
p.colorOnClick = {0.5,0.5,0.5}

function p.update()

end

function p.onMouseDown()
	print("down")
	sprite.color = p.colorOnClick
end

function p.onMouseUp()
	print("up")
	sprite.color = {1,1,1}
end

function p.onMouseLeave()
	print("leave")
	sprite.color = {1,1,1}
end

return p