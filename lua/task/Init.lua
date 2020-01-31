local p = {}

function p.start(args)
	Scene = {}
	Scene.name = Game.activeSceneName
	print("===InitTable===")
	print("===_ENV===")
	for key, value in pairs(_ENV) do
		print(key)
	end

	print("===_G===")
	for key, value in pairs(_G) do
		print(key)
	end
	print("===============")
end

return p