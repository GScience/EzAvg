local p = {}

p.enabled = true
p.totalTime = 0

function p.update()
	p.totalTime = p.totalTime + Time.deltaTime()

	if p.totalTime >= p.waitTime then
		p.enabled = false
	end
end

function p.start(args)
	if args.time == nil then
		logError("未指定time")
		p.waitTime = 0
		return
	end
	p.waitTime = args.time;
end

return p