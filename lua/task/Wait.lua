local p = {}

p.finished = false
p.totalTime = 0
p._spriteName = nil
p._behaviourName = nil

function p.update()

	if p.waitTime ~= nil then
		p.totalTime = p.totalTime + Time.deltaTime()

		if p.totalTime >= p.waitTime then
			p.finished = true
		end

	elseif p._behaviourName ~= nil then
		local sprite
		if p._spriteName == nil then
			sprite = scene
		else
			sprite = scene[p._spriteName]
		end

		local behaviour=sprite[p._behaviourName]
		if behaviour == nil then
			logError("Behaviour not found")
			p.finished = true
			return
		end
		if behaviour.isFinished == nil then
			p.finished = true
			return
		end
		if behaviour.isFinished() then
			if p.keepBehaviour == false then
				behaviour.destroyed = true
			end
			p.finished = true
			return
		end
	end

end

function p.start(args)

	if args.time ~= nil then
		p.waitTime = args.time
		return
	elseif args.behaviour ~= nil then
		p._behaviourName = args.behaviour
		p._spriteName = args.sprite
	else
		logError("未指定time或behaviour")
	end
end

return p