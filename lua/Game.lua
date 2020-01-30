require('Config')

Game = {}

--储存游戏中所有可保存数据
Game.dataTable = {}

function Game.Start()
	print('已启动')
	Application.setTitle(Config.title)
	Application.setWindowSize(Config.windowSize[1], Config.windowSize[2])

	Game.LoadScene('Startup.scene')
end

function Game.Update()
	
end

function Game.LoadScene(sceneName)
	Game.activeSceneName = sceneName
	ActiveScene.scriptRun(sceneName)
end