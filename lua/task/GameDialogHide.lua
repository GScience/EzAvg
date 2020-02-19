local p = {}

p.finished = false

function p.start(args)
	local behaviour = scene._MainDialog.fader
	behaviour.speed = -1
	behaviour.enabled = true
end

function p.update()
	local behaviour = scene._MainDialog.fader
	if (behaviour.isFinished()) then
		scene.text=""
		p.finished = true
	end
end

return p