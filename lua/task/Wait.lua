local p = {}

p.finished = false
p.totalTime = 0

function p.update()

	if p.waitTime ~= nil then
		p.totalTime = p.totalTime + Time.deltaTime()

		if p.totalTime >= p.waitTime then
			p.finished = true
		end

	elseif p.waitBehaviour ~= nil then
		local sprite=scene[p.sptire]
		if sprite == nil then
			p.finished = true
			return
		end
		local behaviour=sprite[p.waitBehaviour]
		if behaviour == nil then
			LogError("Behaviour not found")
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
	end

	if args.sprite ~= nil then
		if args.behaviour ~= nil then
			p.waitBehaviour = args.behaviour
			p.sptire = args.sprite
			if args.keep == nil then
				p.keepBehaviour = false
			else
				p.keepBehaviour = args.keep
			end
			return
		end
	end

	logError("未指定time，或sprite与behaviour二者中缺少一个")
	p.finished = true
end

return p