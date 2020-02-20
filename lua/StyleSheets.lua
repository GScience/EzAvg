
Styles = {}

Styles.GameScene =
{
	-- 文本框
	_MainDialog=
	{
		type = "group",
		zOrder = 1000,
		margin={409,25,25,25},
		alpha=0,
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
		-- 淡入淡出
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
		image="_MainDialog.Text.Image.image"
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
		image="Image.image",
		color="Image.color",
		colorOnClick="buttonBehaviour.colorOnClick",
		colorOnEnter="buttonBehaviour.colorOnEnter",
		colorNormal="buttonBehaviour.colorNormal",
		isClicked="buttonBehaviour._isClicked"
	}
}