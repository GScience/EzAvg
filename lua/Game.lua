require('Config')

Game = {}

--������Ϸ�����пɱ�������
Game.dataTable = {}

function Game.Start()
	print('������')
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