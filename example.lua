local songTagger = require ("taglib")

--[[
  This file shows what TagLib functions are supported by lua-taglib
  and how to use them.
]]

local song = songTagger.new('song.mp3')

if not song:failed() then
    print('Title:'        .. song:title())
    print('Artist: '      .. song:artist())
    print('Album: '       .. song:album())
    print('Genre: '       .. song:genre())
    print('Track: '       .. song:track())
    print('Song Length: ' .. song:length())
else
    print('Failed to read the song file')
end

song:close()