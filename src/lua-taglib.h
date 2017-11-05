#ifndef LUA_TAG_LIB
#define LUA_TAG_LIB

#include <lua.hpp>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/mp4file.h>
#include <taglib/vorbisfile.h>
#include <taglib/mpegfile.h>
#include <taglib/apefile.h>
#include <taglib/flacfile.h>
#include <taglib/mpcfile.h>
#include <taglib/trueaudiofile.h>
#include <taglib/asffile.h>
#include <taglib/wavpackfile.h>
#include <taglib/wavfile.h>
#include <taglib/aifffile.h>
#include <string.h>
#include <stdio.h>
#include "lua-taglib-cover.h"
#include "lua-taglib-magic.h"

#define TAG_READ_WRITE "TAG_READ_WRITE"
#define TAG_ALBUM_ART "TAG_ALBUM_ART"

namespace LuaTagLib {

    struct LuaTag {
        TagLib::File* tagFile;
        std::string* mimeType;
    };

    struct LuaTagAlbumArtwork;

}

#endif
