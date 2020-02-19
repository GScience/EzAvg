local p = {}

-- 是否结束，若为nil则默认结束
p.finished = false

-- 刷新，若无finished则不调用刷新
function p.update()
end

-- 开始
function p.start(args)
end

return p