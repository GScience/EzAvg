require("Style", "scene")

local p = {}

p.finished = false

function p.start(args)
	applyStyle(scene, Styles.GameScene)
end

function p.update()
	p.finished = true
end

return p