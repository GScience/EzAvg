local p = {}

p.enabled = true
p.speed = 1
p._deltaTime = 0
p.currentPos = 1;
p.text = ""
p._text = ""

function p.onSetText(newText)
	p.currentPos = 1
	p._deltaTime = 0
	p._text = newText
	sprite.text = ""
end

function p.update()

	if p.text ~= 0 then
		p.onSetText(p.text)
		p.text = 0
	end

	if p.currentPos > string.len(p._text) then
		return
	end

	p._deltaTime = p._deltaTime + Time.deltaTime()

	if p._deltaTime * p.speed > 1 then
		p._deltaTime = p._deltaTime - 1 / p.speed
		local n = string.byte(p._text, p.currentPos)
		if n <= 0x7F then -- 1 byte
			sprite.text = sprite.text .. string.sub(p._text, p.currentPos, p.currentPos)
			p.currentPos = p.currentPos + 1
		elseif n <= 0xDF then -- 2 byte
			sprite.text = sprite.text .. string.sub(p._text, p.currentPos, p.currentPos + 1)
			p.currentPos = p.currentPos + 2
		elseif n <= 0xEF then -- 3 byte
			sprite.text = sprite.text .. string.sub(p._text, p.currentPos, p.currentPos + 2)
			p.currentPos = p.currentPos + 3
		elseif n< 0xF7 then -- 4 byte 
			sprite.text = sprite.text .. string.sub(p._text, p.currentPos, p.currentPos + 3)
			p.currentPos = p.currentPos + 4
		elseif n < 0xFB then -- 5 byte
			sprite.text = sprite.text .. string.sub(p._text, p.currentPos, p.currentPos + 4)
			p.currentPos = p.currentPos + 5
		else -- 6 byte
			sprite.text = sprite.text .. string.sub(p._text, p.currentPos, p.currentPos + 5)
			p.currentPos = p.currentPos + 6
		end
	end
end

function p.isFinished()
	
	if p.enabled == false then
		return true
	end
	return p.currentPos > string.len(p._text)
end

return p