require("Style")

local p = {}

function p.start(args)

	if args.sprite == nil then
		logError("无法创建精灵，未指定sprite")
		return
	end
	if args.type == nil then
		logError("无法创建精灵，未指定type")
		return
	end
	if scene == nil then
		logError("无法创建精灵，未找到场景")
		return
	end
	if scene[args.sprite] ~= nil then
		logError("无法创建精灵，精灵或属性" .. args.sprite .. "已存在")
		return
	end
	local sprite = scene.addSprite(args.sprite, args.type)
	if sprite == nil then
		logError("无法创建精灵，精灵" .. args.sprite .. "创建失败")
		return
	end

	local style = args.style
	if style ~= nil then
		applyStyle(sprite, Styles[style])
	end

	for k,v in pairs(args) do
		sprite[k] = v
	end
end

return p