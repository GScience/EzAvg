local p = {}

function p.start(args)

	for k,v in pairs(args) do
		scene[k] = v
	end
end

return p