local p = {}

p.enabled = true
p.speed = 1
p.deltaTime = 0
p.currentPos = 1;
p.text = ""

function p.update()

	if p.currentPos >= string.len(p.text) then
		return
	end

	p.deltaTime = p.deltaTime + Time.deltaTime()

	if p.deltaTime * p.speed > 1 then
		p.deltaTime = 0
		local n = string.byte(p.text, p.currentPos)
		if n <= 0x7F then -- 1 byte
			sprite.text = sprite.text .. string.sub(p.text, p.currentPos, p.currentPos)
			p.currentPos = p.currentPos + 1
		elseif n <= 0xDF then -- 2 byte
			sprite.text = sprite.text .. string.sub(p.text, p.currentPos, p.currentPos + 1)
			p.currentPos = p.currentPos + 2
		elseif n <= 0xEF then -- 3 byte
			sprite.text = sprite.text .. string.sub(p.text, p.currentPos, p.currentPos + 2)
			p.currentPos = p.currentPos + 3
		elseif n< 0xF7 then -- 4 byte 
			sprite.text = sprite.text .. string.sub(p.text, p.currentPos, p.currentPos + 3)
			p.currentPos = p.currentPos + 4
		elseif n < 0xFB then -- 5 byte
			sprite.text = sprite.text .. string.sub(p.text, p.currentPos, p.currentPos + 4)
			p.currentPos = p.currentPos + 5
		else -- 6 byte
			sprite.text = sprite.text .. string.sub(p.text, p.currentPos, p.currentPos + 5)
			p.currentPos = p.currentPos + 6
		end
	end
end

function p.start()
end

return p