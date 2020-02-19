require("Style", "scene")

local p = {}

p.finished = false

function p.start(args)
	applyStyle(scene, Styles.GameScene)
end

function p.update()
	local behaviour = scene._MainDialog.fader
	p.finished = behaviour.isFinished()
end

return p