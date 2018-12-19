function parseInputHelper(corpus, n)
    local offset = 0
    local tokens = {}

    local token = ""

    while (token ~= nil) do
        token, offset = parseInput(corpus, offset)

        if token ~= '' then
            table.insert(tokens, token)
        end
    end

    for i = 1, n do
        table.insert(tokens, tokens[i])
    end

    local totaltok = #tokens

    return tokens, totaltok
end

function makeShingles(tokens, totaltok, n)
    local ngrams = {}
    for i = 1, totaltok do
        local gram = {}
        for j = i, i + n-1 do
            table.insert(gram, tokens[j])
        end

        table.insert(ngrams, gram)
    end

    return ngrams
end

function makePrefixTable(shingles)
    local prfxs = {}
    for _, v in pairs(shingles) do
        local key = {table.unpack(v)}
        local suffix = table.remove(key)
        local key = table.concat(key, " ")

        if (prfxs[key] == nil) then
            prfxs[key] = {}
        end

        table.insert(prfxs[key], suffix)
    end

    return prfxs
end

function main(fname, wordcount, n)
    -- The Lua code calls a C function to load the input file, returning it as Lua string
    local corpus = readFile(fname)

    -- The Lua code calls a C function to parse the input string. It passes in the string and an offset (see below) and gets back a single new token and a new offset. It calls this many times, resulting in a list of tokens in Lua.
    local tokens, total = parseInputHelper(corpus, n)

    -- The Lua code generates n-grams, then shingles, then a table of prefixes mapped to last words (see below).
    local shingles = makeShingles(tokens, total, n)

    local prefixes = makePrefixTable(shingles)

    -- The Lua code babbles the requested number of words to the console

    math.randomseed(os.time())
    
    local shingle = shingles[math.random(#shingles)]

    for i = 0, wordcount do
        table.remove(shingle, 1)
        local prefix = table.concat(shingle, " ")
        local cands = prefixes[prefix]
        local suffix = cands[math.random(#cands)]
        table.insert(shingle, suffix)

        io.write(suffix .. " ")
    end
end