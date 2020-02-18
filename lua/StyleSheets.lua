
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
		fontSize = 24,
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
		colorOnClick="buttonBehaviour.colorOnClick"
	}
}