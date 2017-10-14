lua-taglib
==========

Lua library for reading metadata from audio files

The library uses the TagLib (http://taglib.github.com/) C++ library.
Not all TagLib funtionality has been implemented, you can only read metadata not write etc. etc.

lua-taglib is very early in development, expect errors!

## Requirements ##
* lua5.2
* liblua5.2-0-dev
* libmagic-dev
* TagLib (libtag1-dev)

## Documentation ##

### Building using Luarocks ###

```bash
git clone repo

cd repo

luarocks make
```

### Available functions

```lua
taglib.new(string : filename) : userdata | nil string
```

Creates a new taglib userdata to retrieve music metadata. On failure it returns nil and an error message. Will throw a catchable error if the filename argument is not provided or is not a string.

All userdata methods throw catchable errors if their first argument is not the userdata itself.

```lua
song.artist(song) : string
song:artist() : string
```

Retrieves the songs artist. If the tag does not exist it will return an empty string

```lua
song.album(song) : string
song:album() : string
```

Retrieves the songs album name. If the tag does not exist it will return an empty string

```lua
song.title(song) : string
song:title() : string
```

Retrieves the songs title. If the tag does not exist it will return an empty string

```lua
song.genre(song) : string
song:genre() : string
```

Retrieves the songs genre. If the tag does not exist it will return an empty string

```lua
song.track(song) : number
song:track() : number
```

Retrieves the songs track number.

```lua
song.length(song) : number
song:length() : number
```

Retrieves the length of the song in seconds.

```lua
song.year(song) : number
song:length() : number
```

Retrieves the year the song was made.

```lua
song.mimeType(song) : string
song:mimeType() : string
```

Retrieves the mime type of the provided song file. Mainly used a debugging libmagic functionality.

```lua
song.close(song)
song:close()
```

Closes the song and frees the memory used by the associated objects. This function can be called explicity or will be called when lua runs its garbage collector.

## TODO ##

Add Lua support for more of the TagLib library functions

More testing...
I have only testet the library using a single mp3 file on my Ubuntu 12.10 Linux...
