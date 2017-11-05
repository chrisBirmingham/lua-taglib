#include "lua-taglib.h"

using LuaTagLib::LuaTag;
using LuaTagLib::LuaTagAlbumArtwork;

struct LuaTagAlbumArtwork {
    LuaTagLib::Cover::Artwork* artwork;
    std::string* mimeType;
};

/**
 * Checks whether the first argument to a function of the userdata TAG_READ_WRITE
 */
static LuaTag* tagCheck(lua_State* L)
{
    void* readWriter = luaL_checkudata(L, 1, TAG_READ_WRITE);
    luaL_argcheck(L, readWriter != NULL, 1, "`tag class' expected");
    return (LuaTag*)readWriter;
}

/**
 * Creates a tag reader userdata
 */
static LuaTag* tagCreate(lua_State* L)
{
    LuaTag* readWriter = (LuaTag*)lua_newuserdata(L, sizeof(LuaTag));
    luaL_getmetatable(L, TAG_READ_WRITE);
    lua_setmetatable(L, -2);
    return readWriter;
}

/**
 * Checks whether the first argument to a function of the userdata TAG_READ_WRITE
 */
LuaTagAlbumArtwork* coverCheck(lua_State* L)
{
    void* artwork = luaL_checkudata(L, 1, TAG_ALBUM_ART);
    luaL_argcheck(L, artwork != NULL, 1, "`album class' expected");
    return (LuaTagAlbumArtwork*)artwork;
}

/**
 * Creates a tag reader userdata
 */
LuaTagAlbumArtwork* coverCreate(lua_State* L)
{
    LuaTagAlbumArtwork* album = (LuaTagAlbumArtwork*)lua_newuserdata(L, sizeof(LuaTagAlbumArtwork));
    luaL_getmetatable(L, TAG_ALBUM_ART);
    lua_setmetatable(L, -2);
    return album;
}

/**
 * Creates the desired taglib object based on the files mimetype
 */
static TagLib::File* createTagLibObj(const char* file, const std::string mime) {
    TagLib::File* tagFile = NULL;

    if (mime == "audio/mp4") {
        tagFile = new TagLib::MP4::File(file);
    } else if (mime == "audio/ogg") {
        tagFile = new TagLib::Ogg::Vorbis::File(file);
    } else if (mime == "audio/mpeg") {
        tagFile = new TagLib::MPEG::File(file);
    } else if (mime == "audio/flac") {
        tagFile = new TagLib::FLAC::File(file);
    } else if (mime == "audio/x-musepack") {
        tagFile = new TagLib::MPC::File(file);
    } else if (mime == "audio/musepack") {
        tagFile = new TagLib::MPC::File(file);
    } else if (mime == "audio/x-tta") {
        tagFile = new TagLib::TrueAudio::File(file);
    } else if (mime == "audio/x-monkeys-audio") {
        tagFile = new TagLib::APE::File(file);
    } else if (mime == "audio/x-ms-wma") {
        tagFile = new TagLib::ASF::File(file);
    } else if (mime == "audio/x-wv") {
        tagFile = new TagLib::WavPack::File(file);
    } else if (mime == "audio/x-wav") {
        tagFile = new TagLib::RIFF::WAV::File(file);
    } else if (mime == "audio/x-aiff") {
        tagFile = new TagLib::RIFF::AIFF::File(file);
    }

    return tagFile;
}

/**
 * Creates a tag reader userdata and associate a tagfile with it
 */
static int tagNew(lua_State* L)
{
    const char* file = luaL_checkstring(L, 1);
    const char* mime;

    if (strcmp(file, "") == 0) {
        lua_pushnil(L);
        lua_pushstring(L, "Provided filename cannot be empty");
        return 2;
    }

    std::string mimetype;
    LuaTagLib::Magic::FileTypeResolver resolver;
    bool res;

    if ((res = resolver.initialiseMagic()) == false) {
        lua_pushfstring(L, "Error initialising lib magic with error `%s`", resolver.getErrorMessage().c_str());
        lua_pushnil(L);
        return 2;
    }

    std::string mimeType;
    if ((mimeType = resolver.detectByFilePath(file)) == "") {
        lua_pushnil(L);
        lua_pushfstring(L, "Error detecting file mimetype with error `%s`", resolver.getErrorMessage().c_str());
        return 2;
    }

    TagLib::File* tagFile;

    if ((tagFile = createTagLibObj(file, mimeType)) == NULL) {
        lua_pushnil(L);
        lua_pushfstring(L, "Invalid mimetype for file `%s` detected `%s`", file, mimeType);
        return 2;
    }

    if (!tagFile->isValid()) {
        lua_pushnil(L);
        lua_pushstring(L, "Invalid file provided to taglib");
        free(tagFile);
        return 2;
    }

    LuaTag* readWriter = tagCreate(L);
    readWriter->tagFile        = tagFile;
    readWriter->mimeType       = new std::string(mimeType.c_str(), strlen(mimeType.c_str()));

    return 1;
}

/**
 * Pushes a taglib string onto the lua stack. If the string is null converts to
 * an empty string
 */
static void pushTagLibString(lua_State* L, TagLib::String str)
{
    if (str.isNull()) {
        lua_pushstring(L, "");
    } else {
        lua_pushstring(L, str.toCString());
    }
}

/**
    Retrieves the song artist
*/
static int tagArtist(lua_State* L)
{
    LuaTag* reader = tagCheck(L);
    TagLib::File* f = reader->tagFile;
    pushTagLibString(L, f->tag()->artist());
    return 1;
}

/**
    Retrieves the songs album
*/
static int tagAlbum(lua_State* L)
{
    LuaTag* reader = tagCheck(L);
    TagLib::File* f = reader->tagFile;
    pushTagLibString(L, f->tag()->album());
    return 1;
}

/**
    Retrieves the songs title
*/
static int tagTitle(lua_State* L)
{
    LuaTag* reader = tagCheck(L);
    TagLib::File* f = reader->tagFile;
    pushTagLibString(L, f->tag()->title());
    return 1;
}

/**
    Retrieves the songs track
*/
static int tagTrack(lua_State* L)
{
    LuaTag* reader = tagCheck(L);
    TagLib::File* f = reader->tagFile;
    lua_pushinteger(L, f->tag()->track());
    return 1;
}

/**
    Retrieves the songs year
*/
static int tagYear(lua_State* L)
{
    LuaTag* reader = tagCheck(L);
    TagLib::File* f = reader->tagFile;
    lua_pushinteger(L, f->tag()->year());
    return 1;
}

/**
    Retrieves the songs genre
*/
static int tagGenre(lua_State* L)
{
    LuaTag* reader = tagCheck(L);
    TagLib::File* f = reader->tagFile;
    pushTagLibString(L, f->tag()->genre());
    return 1;
}

/**
 * Retrive the length of the song
 */
static int tagLength(lua_State* L)
{
    LuaTag* reader = tagCheck(L);
    TagLib::File* f = reader->tagFile;
    lua_pushinteger(L, f->audioProperties()->length());
    return 1;
}

/**
 * Retrieves the mimetype of the audio file
 */
static int tagMimeType(lua_State* L)
{
    LuaTag* reader = tagCheck(L);
    lua_pushstring(L, reader->mimeType->c_str());
    return 1;
}

static int tagAlbumArtwork(lua_State* L)
{
    LuaTag* reader = tagCheck(L);

    LuaTagLib::Cover::Artwork* art = new LuaTagLib::Cover::Artwork;

    if (art->getAlbumArtwork(reader->tagFile, *reader->mimeType)) {
        LuaTagLib::Magic::FileTypeResolver resolver;
        bool res;

        if ((res = resolver.initialiseMagic()) == false) {
            lua_pushfstring(L, "Error initialising lib magic with error `%s`", resolver.getErrorMessage().c_str());
            lua_pushnil(L);
            free(art);
            return 2;
        }

        std::string mimeType;
        if ((mimeType = resolver.detectByBuffer(art->getData()->data(), art->getData()->size())) == "") {
            lua_pushnil(L);
            lua_pushfstring(L, "Error detecting cover artwork mimetype with error `%s`", resolver.getErrorMessage().c_str());
            free(art);
            return 2;
        }

        LuaTagAlbumArtwork* album = coverCreate(L);
        album->artwork            = art;
        album->mimeType           = new std::string(mimeType.c_str(), strlen(mimeType.c_str()));
        return 1;
    }

    lua_pushnil(L);
    lua_pushstring(L, art->getErrorMessage().c_str());
    free(art);
    return 2;
}

/**
 * Frees the file object stored in the userdata
 */
static int tagClose(lua_State* L)
{
    LuaTag* reader = tagCheck(L);
    free(reader->tagFile);
    free(reader->mimeType);
    reader->tagFile = NULL;
}

/**
 * Called when lua runs the garbage collector for the userdata.
 * Ensures that the file object is freed if the user does not call
 * the close function explicitly
 */
static int tagGarbageCollect(lua_State* L)
{
    LuaTag* reader = tagCheck(L);
    if (reader->tagFile != NULL) {
        free(reader->tagFile);
        free(reader->mimeType);
    }
}

static int coverData(lua_State* L)
{
    LuaTagAlbumArtwork* art = coverCheck(L);
    lua_pushlstring(L, art->artwork->getData()->data(), art->artwork->getData()->size());
    return 1;
}

static int coverMimetype(lua_State* L)
{
    LuaTagAlbumArtwork* art = coverCheck(L);
    lua_pushstring(L, art->mimeType->c_str());
    return 1;
}

static int coverSize(lua_State* L)
{
    LuaTagAlbumArtwork* art = coverCheck(L);
    lua_pushinteger(L, art->artwork->getData()->size());
    return 1;
}

static int coverGarbageCollect(lua_State* L)
{
    LuaTagAlbumArtwork* art = coverCheck(L);
    free(art->artwork);
    free(art->mimeType);
}

/* Define the function names for lua to call */
static const luaL_Reg LuaTagLibAPI[] = {
    {"new", tagNew},
    { NULL, NULL },
};

/* Define the lua mappings for the userdata methods */
static const luaL_Reg TagMethods[] = {
    {"artist",       tagArtist},
    {"album",        tagAlbum},
    {"title",        tagTitle},
    {"track",        tagTrack},
    {"year",         tagYear},
    {"genre",        tagGenre},
    {"length",       tagLength},
    {"albumArtwork", tagAlbumArtwork},
    {"mimeType",     tagMimeType},
    {"close",        tagClose},
    {"__gc",         tagGarbageCollect},
    {NULL,           NULL}
};

static const luaL_Reg tagAlbumArtworkMethods[] = {
    {"data",     coverData},
    {"mimeType", coverMimetype},
    {"size",     coverSize},
    {"__gc",     coverGarbageCollect},
    {NULL,       NULL}
};

LUALIB_API int luaopen_taglib(lua_State* L)
{
    /* Create the metatable for the userdata and assign function calls to it */

    /* Assign the methods for the album artwork userdata */
    luaL_newmetatable(L, TAG_ALBUM_ART);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, tagAlbumArtworkMethods, 0);

    /* Assign the methods for the tag read writer userdata */
    luaL_newmetatable(L, TAG_READ_WRITE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, TagMethods, 0);

    /* Assign the global fucntions to the library on require */
    luaL_newlib(L, LuaTagLibAPI);
    return 1;
}
