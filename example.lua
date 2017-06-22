local songReader = require ("taglib.tagReader")

--[[
  This file shows what TagLib functions are supported by lua-taglib
  and how to use them.
]]

-- File exists
song = songReader.new('song.mp3')

tags = song:tags()
print("Artist: " .. tags.artist)
print("Album: " .. tags.album)
print("Title: " .. tags.title)
print("Genre: " .. tags.genre)
print("Track: " .. tags.track)
print("Year: " .. tags.year)
print("Comment: " .. tags.comment)

-- audio values
print("Length: " .. song:length())
--print("Bitrate: " .. taglib.bitrate())
--print("Sample Rate: " .. taglib.sampleRate())
--print("Channels: " .. taglib.channels())
--

song:close()

-- File does not exist
song = songReader.new('fake.mp3')

tags = song:tags()

if tags == nil then
    print('File does not exist')
end

song:close()
