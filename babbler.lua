-- checks if token is proj gutenberg page num of format "12345m"
function isNotGutenbergPageNum(token)
    if #token == 5 or #token == 6 then
        local char = token:sub(#token,#token)
        if char == 'm' then
            for i = 1, #token - 1 do
                char = token:sub(i,i)
                if tonumber(char) == nil then
                    return true
                end
            end
            return false
        end
        return true
    end
    return true
end

function parseInputHelper(corpus, n)
    local offset = 0
    local tokens = {}

    local token = ""

    while offset < #corpus do
        token, offset = parseInput(corpus, offset)

        if token ~= '' and isNotGutenbergPageNum(token) then
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
    -- Calls C function to load the input file, returning it as Lua string
    local corpus = readFile(fname)

    -- Call C function to parse the input string
    local tokens, total = parseInputHelper(corpus, n)

    -- Generates n-grams, then shingles, then a table of prefixes mapped to last words
    local shingles = makeShingles(tokens, total, n)

    local prefixes = makePrefixTable(shingles)

    -- Babble the requested number of words to the console

    -- Find random first shingle
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