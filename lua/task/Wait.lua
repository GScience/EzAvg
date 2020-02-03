local p = {}

p.enabled = true
p.totalTime = 0

function p.update()

	if p.waitTime ~= nil then
		p.totalTime = p.totalTime + Time.deltaTime()

		if p.totalTime >= p.waitTime then
			p.enabled = false
		end

	elseif p.waitBehaviour ~= nil then
		local sprite=scene[p.sptire]
		if sprite == nil then
			p.enabled = false
			return
		end
		local behaviour=sprite[p.waitBehaviour]
		if behaviour == nil then
			p.enabled = false
			return
		end
		if behaviour.isFinished == nil then
			p.enabled = false
			return
		end
		if behaviour.isFinished() then
			if p.keepBehaviour ~= true then
				behaviour.enabled = false
			end
			p.enabled = false
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
	p.enabled = false
end

return p