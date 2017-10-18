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
#include <magic.h>
#include <string.h>
#include "lua-taglib-cover.h"

#define TAG_READ_WRITE "TAG_READ_WRITE"

struct tag {
    TagLib::File* tagFile;
    std::string* mimeType;
    bool closed;
};

typedef tag Tag;

static Tag* checkReadWriter(lua_State* L);

static Tag* createReadWriter(lua_State* L);

static int new_tagReadWriter(lua_State* L);

static void pushTagLibString(lua_State* L, TagLib::String str);

static int artist(lua_State* L);

static int album(lua_State* L);

static int title(lua_State* L);

static int track(lua_State* L);

static int year(lua_State* L);

static int genre(lua_State* L);

static int length(lua_State* L);

static int albumArtwork(lua_State* L);

static int free_tagReadWriter(lua_State* L);

static int garbage_collect_tagReadWriter(lua_State* L);

LUALIB_API int luaopen_taglib(lua_State* L);

#endif
