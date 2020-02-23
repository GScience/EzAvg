require("Style", "scene")

local p = {}

p.finished = false

function p.start(args)
	applyStyle(scene, Styles.GameScene)
	for k,v in pairs(args) do
		scene[k] = v
	end
end

function p.update()
	local behaviour = scene.fader
	p.finished = behaviour.isFinished()
end

return p