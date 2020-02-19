require("Style", "scene")

local p = {}

p.finished = false

function p.start(args)
	applyStyle(scene, Styles.GameScene)
	scene.alpha=0
	local behaviour = scene.addBehaviour("_fadeIn", "AnimFadeIn")
	behaviour.speed = 1
end

function p.update()
	local behaviour = scene._fadeIn
	p.finished = behaviour.isFinished()
	if p.finished == true then
		behaviour.destroyed = true
	end
end

return p