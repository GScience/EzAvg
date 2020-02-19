
Styles = {}

Styles.GameScene =
{
	_MainText=
	{
		type = "text",
		order = 10000,
		behaviours = 
		{
			textPrinter =
			{
				type="AnimTextPrinter",
				speed=10
			}
		}
	},
	backgroundColor={1,1,1},
	propertyTable=
	{
		text="_MainText.textPrinter.text",
		printedAllText="_MainText.textPrinter.isFinished"
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
		isClicked="buttonBehaviour._isClicked"
	}
}