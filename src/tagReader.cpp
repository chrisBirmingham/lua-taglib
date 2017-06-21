#include <lua.hpp>

#include <taglib/fileref.h>
#include <taglib/tag.h>

#define TAG_READER "TagReader"

typedef struct tagReader {
    TagLib::FileRef* f;
} TagReader;

/**
 * Checks whether the first argument to a function of the userdata tag reader
 */
static TagReader* checkReader(lua_State* L)
{
    void* reader = luaL_checkudata(L, 1, TAG_READER);
    luaL_argcheck(L, reader != NULL, 1, "`tag reader' expected");
    return (TagReader*)reader;
}

/**
 * Creates a tag reader userdata
 */
static TagReader* createReader(lua_State* L)
{
    TagReader* reader = (TagReader*)lua_newuserdata(L, sizeof(TagReader) + sizeof(TagLib::FileRef*));
    luaL_getmetatable(L, TAG_READER);
    lua_setmetatable(L, -2);
    return reader;
}

/**
 * Creates a tag reader userdata and associate a tagfile with it
 */
static int new_tagReader(lua_State* L)
{
    const char* file = luaL_checkstring(L, 1);
    TagReader* reader = createReader(L);
    reader->f = new TagLib::FileRef(file);
    return 1;
}

/**
 * Retrives a song file metadata tags
 */
static int tags(lua_State *L)
{
    TagReader* reader = checkReader(L);
    TagLib::FileRef* f = reader->f;
    if(!f->isNull() && f->tag()) {
        lua_newtable(L);
        // ARTIST
        lua_pushstring(L, "artist");
        lua_pushstring(L, f->tag()->artist().toCString());
        lua_rawset(L, -3);
        // ALBUM
        lua_pushstring(L, "album");
        lua_pushstring(L, f->tag()->album().toCString());
        lua_rawset(L, -3);
        // TITLE
        lua_pushstring(L, "title");
        lua_pushstring(L, f->tag()->title().toCString());
        lua_rawset(L, -3);
        // GENRE
        lua_pushstring(L, "genre");
        lua_pushstring(L, f->tag()->genre().toCString());
        lua_rawset(L, -3);
        // TRACKNUMBER
        lua_pushstring(L, "track");
        lua_pushinteger(L, f->tag()->track());
        lua_rawset(L, -3);
        // DATE
        lua_pushstring(L, "year");
        lua_pushinteger(L, f->tag()->year());
        lua_rawset(L, -3);
        // COMMENT
        lua_pushstring(L, "comment");
        lua_pushstring(L, f->tag()->comment().toCString(true));
        lua_rawset(L, -3);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * Retrive the length of the song
 */
static int length(lua_State* L)
{
    TagReader* reader = checkReader(L);
    TagLib::FileRef* f = reader->f;
    if (!f->isNull()) {
        lua_pushinteger(L, f->audioProperties()->length());
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * Frees the file object stored in the userdata
 */
static int free_tagReader(lua_State* L)
{
    TagReader* reader = checkReader(L);
    free(reader->f);
}

/**static int bitrate(lua_State *L)
{
    lua_pushinteger(L, f.audioProperties()->bitrate());
    return 1;
}
static int sampleRate(lua_State *L)
{
    lua_pushinteger(L, f.audioProperties()->sampleRate());
    return 1;
}

static int channels(lua_State *L)
{
    lua_pushinteger(L, f.audioProperties()->channels());
    return 1;
}*/

/** Define the function names for lua to call */
static const luaL_Reg tag_reader_f[] = {
    {"tags", tags},
    {"length", length},
    {"new", new_tagReader},
    {"free", free_tagReader},
    { NULL, NULL },
};

/** Define the lua mappings for the userdata */
static const luaL_Reg tag_reader_m[] = {
    {"tags", tags},
    {"length", length},
    {NULL, NULL}    
};

LUALIB_API "C" int luaopen_taglib_tagReader(lua_State* L)
{
    luaL_newmetatable(L, TAG_READER);
    lua_newtable(L);
    luaL_setfuncs(L, tag_reader_f, 0);
    return 1;
}
