require('Config')

Game = {}

function Game.start()
	print('已启动')
	Application.setTitle(Config.title)
	Application.setApplicationSize(Config.applicationSize[1], Config.applicationSize[2])

	Game.loadScene('game/GameFirstScene.scene')
end

function Game.update()
	
end

function Game.loadScene(sceneName)
	Game.activeSceneName = sceneName
	Application.loadScene(sceneName)
end