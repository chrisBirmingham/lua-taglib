#include <lua.hpp>

#include <taglib/fileref.h>
#include <taglib/tag.h>

#define TAG_READ_WRITE "TAG_READ_WRITE"

struct tag {
    TagLib::FileRef* tagFile;
    bool failed;
    bool closed;
};

typedef tag Tag;

/**
 * Checks whether the first argument to a function of the userdata TAG_READ_WRITE
 */
static Tag* checkReadWriter(lua_State* L)
{
    void* readWriter = luaL_checkudata(L, 1, TAG_READ_WRITE);
    luaL_argcheck(L, readWriter != NULL, 1, "`tag class' expected");
    return (Tag*)readWriter;
}

/**
 * Creates a tag reader userdata
 */
static Tag* createReadWriter(lua_State* L)
{
    Tag* readWriter = (Tag*)lua_newuserdata(L, sizeof(Tag) + sizeof(TagLib::FileRef*));
    luaL_getmetatable(L, TAG_READ_WRITE);
    lua_setmetatable(L, -2);
    return readWriter;
}

/**
 * Creates a tag reader userdata and associate a tagfile with it
 */
static int new_tagReadWriter(lua_State* L)
{
    const char* file    = luaL_checkstring(L, 1);
    Tag* readWriter     = createReadWriter(L);
    readWriter->tagFile = new TagLib::FileRef(file);
    readWriter->failed  = (readWriter->tagFile->isNull() && !readWriter->tagFile->tag());
    readWriter->closed  = false;
    return 1;
}

static int failed(lua_State *L)
{
    Tag* reader = checkReadWriter(L);
    lua_pushboolean(L, reader->failed);
    return 1;
}

/**
    Retrieves the song artist
*/
static int artist(lua_State *L)
{
    Tag* reader = checkReadWriter(L);
    if(!reader->failed && !reader->closed) {
        TagLib::FileRef* f = reader->tagFile;
        lua_pushstring(L, f->tag()->artist().toCString());
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
    Retrieves the songs album
*/
static int album(lua_State *L)
{
    Tag* reader = checkReadWriter(L);
    if(!reader->failed and !reader->closed) {
        TagLib::FileRef* f = reader->tagFile;
        lua_pushstring(L, f->tag()->album().toCString());
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
    Retrieves the songs title
*/
static int title(lua_State *L)
{
    Tag* reader = checkReadWriter(L);
    if(!reader->failed && !reader->closed) {
        TagLib::FileRef* f = reader->tagFile;
        lua_pushstring(L, f->tag()->title().toCString());
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
    Retrieves the songs track
*/
static int track(lua_State *L)
{
    Tag* reader = checkReadWriter(L);
    if(!reader->failed && !reader->closed) {
        TagLib::FileRef* f = reader->tagFile;
        lua_pushinteger(L, f->tag()->track());
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
    Retrieves the songs year
*/
static int year(lua_State *L)
{
    Tag* reader = checkReadWriter(L);
    if(!reader->failed && !reader->closed) {
        TagLib::FileRef* f = reader->tagFile;
        lua_pushinteger(L, f->tag()->year());
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
    Retrieves the songs genre
*/
static int genre(lua_State *L)
{
    Tag* reader = checkReadWriter(L);
    if(!reader->failed && !reader->closed) {
        TagLib::FileRef* f = reader->tagFile;
        lua_pushstring(L, f->tag()->genre().toCString());
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
    Tag* reader = checkReadWriter(L);
    if (!reader->failed && !reader->closed) {
        TagLib::FileRef* f = reader->tagFile;
        lua_pushinteger(L, f->audioProperties()->length());
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * Frees the file object stored in the userdata
 */
static int free_tagReadWriter(lua_State* L)
{
    Tag* reader = checkReadWriter(L);
    free(reader->tagFile);
    reader->tagFile = NULL;
}

/**
 * Called when lua runs the garbage collector for the userdata.
 * Ensures that the file object is freed if the user does not call
 * the close function explicitly
 */
static int garbage_collect_tagReadWriter(lua_State* L)
{
    Tag* reader = checkReadWriter(L);
    if (reader->tagFile != NULL) {
        free(reader->tagFile);
        reader->tagFile = NULL;
    }
}

/* Define the function names for lua to call */
static const luaL_Reg tag_functions[] = {
    {"new", new_tagReadWriter},
    { NULL, NULL },
};

/* Define the lua mappings for the userdata methods */
static const luaL_Reg tag_methods[] = {
    {"failed", failed},
    {"artist", artist},
    {"album",  album},
    {"title",  title},
    {"track",  track},
    {"year",   year},
    {"genre",  genre},
    {"length", length},
    {"close",  free_tagReadWriter},
    {"__gc",   garbage_collect_tagReadWriter},
    {NULL,     NULL}
};

LUALIB_API "C" int luaopen_taglib(lua_State* L)
{
    /* Create the metatable for the userdata and assign function calls to it */
    luaL_newmetatable(L, TAG_READ_WRITE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, tag_methods, 0);
    luaL_newlib(L, tag_functions);
    return 1;
}
