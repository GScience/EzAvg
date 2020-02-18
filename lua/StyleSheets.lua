
Styles = {}

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
		color = {0.8,0.8,0.8},
		shadowColor = {0.7,0.7,0.7},
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