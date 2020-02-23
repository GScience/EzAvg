
Styles = {}

Styles.GameScene =
{
	alpha=0,
	Background=
	{
		type = "image",
		zOrder = -1
	},
	-- 文本框
	_MainDialog=
	{
		type = "group",
		zOrder = 1000,
		margin={409,25,25,25},
		-- 背景
		Image=
		{
			type = "image",
			zOrder = 1,
			image="UI/Dialog718x462.png"
		},
		-- 文本
		Text=
		{
			type = "text",
			zOrder = 2,
			margin={20,35,20,35},
			fontSize = 30,
			behaviours = 
			{
				textPrinter =
				{
					type="AnimTextPrinter",
					speed=10
				}
			}
		},
		behaviours = 
		{
			fader =
			{
				type="AnimFadeIn",
				speed=1
			}
		}
	},
	propertyTable=
	{
		text="_MainDialog.Text.textPrinter.text",
		printedAllText="_MainDialog.Text.textPrinter.isFinished",
		background="Background.image"
	},
	behaviours = 
	{
		SceneBehaviour =
		{
			type="SceneGame"
		},
		-- 淡入淡出
		fader =
		{
			type="AnimFadeIn",
			speed=1
		}
	}
}

Styles.MessageBoxPopScene =
{
	alpha=0,
	-- 对话框背景
	MsgBoxBackground=
	{
		type = "image",
		image="UI/MessageBox650x400.png",
		margin={184, 187},
		zOrder=0,
		size={650, 400}
	},
	-- 对话框文本
	MsgBoxMessage=
	{
		type = "text",
		verticalLayout="center",
		horizontalLayout="center",
		margin={189, 237},
		zOrder=1,
		size={550, 250}
	},
	-- 对话框按钮
	ButtonGroup=
	{
		type = "group",
		margin={495, 187, 185, 187},
		zOrder=2,
		behaviours=
		{
			layoutBehaviour=
			{
				type="LayoutHorizontal",
				verticalLayout="top"
			}
		}
	},
	behaviours = 
	{
		-- 淡入淡出
		fader =
		{
			type="AnimFadeIn",
			speed=10
		},
		-- 拖动
		drager =
		{
			type="ControlDragable"
		}
	},
	propertyTable =
	{
		text="MsgBoxMessage.text"
	}
}

Styles.Button = 
{
	Image = 
	{
		type = "image",
		zOrder = 0
	},
	Text = 
	{
		type = "text",
		verticalLayout = "center",
		horizontalLayout = "center",
		color = {0.9,0.9,0.9},
		shadowColor = {0.6,0.6,0.6},
		fontSize = 26,
		zOrder = 1
	},
	behaviours = 
	{
		buttonBehaviour=
		{
			type="ControlButton"
		}
	},
	propertyTable=
	{
		text="Text.text",
		fontSize="Text.fontSize",
		image="Image.image",
		color="Image.color",
		colorOnClick="buttonBehaviour.colorOnClick",
		colorOnEnter="buttonBehaviour.colorOnEnter",
		colorNormal="buttonBehaviour.colorNormal",
		isClicked="buttonBehaviour._isClicked"
	}
}