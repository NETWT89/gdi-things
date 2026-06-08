-- i made this in 3 minutes.
math.randomseed(os.time())

local CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()"

local function generatePassword(length)
    local result = {}

    for i = 1, length do
        local index = math.random(#CHARSET)
        result[i] = CHARSET:sub(index, index)
    end

    return table.concat(result)
end

print("Enter password length:")
local length = tonumber(io.read())

if length and length > 0 then
    print("Generated Password: " .. generatePassword(length))
else
    print("Invalid length.")
end
