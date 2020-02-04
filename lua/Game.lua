require('Config')

Game = {}

function Game.start()
	print('已启动')
	Application.setTitle(Config.title)
	Application.setWindowSize(Config.windowSize[1], Config.windowSize[2])

	Game.loadScene('Startup.scene')
end

function Game.update()
	
end

function Game.loadScene(sceneName)
	Game.activeSceneName = sceneName
	Application.loadScene(sceneName)
end