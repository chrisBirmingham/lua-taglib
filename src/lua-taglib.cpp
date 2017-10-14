#include "lua-taglib.h"

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
    Tag* readWriter = (Tag*)lua_newuserdata(L, sizeof(Tag) + sizeof(TagLib::File*));
    luaL_getmetatable(L, TAG_READ_WRITE);
    lua_setmetatable(L, -2);
    return readWriter;
}

static TagLib::File* createTagLibObj(const char* file, const char* mime) {
    TagLib::File* tagFile = NULL;

    if (strcmp(mime, "audio/mp4") == 0) {
        tagFile = new TagLib::MP4::File(file);
    } else if (strcmp(mime, "audio/ogg") == 0) {
        tagFile = new TagLib::Ogg::Vorbis::File(file);
    } else if (strcmp(mime, "audio/mpeg") == 0) {
        tagFile = new TagLib::MPEG::File(file);
    } else if (strcmp(mime, "audio/flac") == 0) {
        tagFile = new TagLib::FLAC::File(file);
    } else if (strcmp(mime, "audio/x-musepack") == 0) {
        tagFile = new TagLib::MPC::File(file);
    } else if (strcmp(mime, "audio/musepack") == 0) {
        tagFile = new TagLib::MPC::File(file);
    } else if (strcmp(mime, "audio/x-tta") == 0) {
        tagFile = new TagLib::TrueAudio::File(file);
    } else if (strcmp(mime, "audio/x-monkeys-audio") == 0) {
        tagFile = new TagLib::APE::File(file);
    } else if (strcmp(mime, "audio/x-ms-wma") == 0) {
        tagFile = new TagLib::ASF::File(file);
    } else if (strcmp(mime, "audio/x-wv") == 0) {
        tagFile = new TagLib::WavPack::File(file);
    } else if (strcmp(mime, "audio/x-wav") == 0) {
        tagFile = new TagLib::RIFF::WAV::File(file);
    } else if (strcmp(mime, "audio/x-aiff") == 0) {
        tagFile = new TagLib::RIFF::AIFF::File(file);
    }

    return tagFile;
}

/**
 * Creates a tag reader userdata and associate a tagfile with it
 */
static int new_tagReadWriter(lua_State* L)
{
    const char* file = luaL_checkstring(L, 1);
    const char* mime;
    magic_t magicCookie;

    if ((magicCookie = magic_open(MAGIC_MIME_TYPE | MAGIC_ERROR)) == NULL) {
        lua_pushnil(L);
        lua_pushstring(L, magic_error(magicCookie));
        return 2;
    }

    if (magic_load(magicCookie, NULL) != 0) {
        lua_pushnil(L);
        lua_pushstring(L, magic_error(magicCookie));
        magic_close(magicCookie);
        return 2;
    }

    if ((mime = magic_file(magicCookie, file)) == NULL) {
        lua_pushnil(L);
        lua_pushstring(L, magic_error(magicCookie));
        magic_close(magicCookie);
        return 2;
    }

    TagLib::File* tagFile;

    if ((tagFile = createTagLibObj(file, mime)) == NULL) {
        lua_pushnil(L);
        lua_pushstring(L, "Invalid mime type provided");
        magic_close(magicCookie);
        return 2;
    }

    if (!tagFile->isValid()) {
        lua_pushnil(L);
        lua_pushstring(L, "The input file provided is not valid");
        magic_close(magicCookie);
        return 2;
    }

    Tag* readWriter      = createReadWriter(L);
    readWriter->tagFile  = tagFile;
    readWriter->mimeType = new std::string(mime, strlen(mime));
    readWriter->closed   = false;

    magic_close(magicCookie);

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
static int artist(lua_State* L)
{
    Tag* reader = checkReadWriter(L);
    TagLib::File* f = reader->tagFile;
    pushTagLibString(L, f->tag()->artist());
    return 1;
}

/**
    Retrieves the songs album
*/
static int album(lua_State* L)
{
    Tag* reader = checkReadWriter(L);
    TagLib::File* f = reader->tagFile;
    pushTagLibString(L, f->tag()->album());
    return 1;
}

/**
    Retrieves the songs title
*/
static int title(lua_State* L)
{
    Tag* reader = checkReadWriter(L);
    TagLib::File* f = reader->tagFile;
    pushTagLibString(L, f->tag()->title());
    return 1;
}

/**
    Retrieves the songs track
*/
static int track(lua_State* L)
{
    Tag* reader = checkReadWriter(L);
    TagLib::File* f = reader->tagFile;
    lua_pushinteger(L, f->tag()->track());
    return 1;
}

/**
    Retrieves the songs year
*/
static int year(lua_State* L)
{
    Tag* reader = checkReadWriter(L);
    TagLib::File* f = reader->tagFile;
    lua_pushinteger(L, f->tag()->year());
    return 1;
}

/**
    Retrieves the songs genre
*/
static int genre(lua_State* L)
{
    Tag* reader = checkReadWriter(L);
    TagLib::File* f = reader->tagFile;
    pushTagLibString(L, f->tag()->genre());
    return 1;
}

/**
 * Retrive the length of the song
 */
static int length(lua_State* L)
{
    Tag* reader = checkReadWriter(L);
    TagLib::File* f = reader->tagFile;
    lua_pushinteger(L, f->audioProperties()->length());
    return 1;
}

static int mimeType(lua_State* L)
{
    Tag* reader = checkReadWriter(L);
    lua_pushstring(L, reader->mimeType->c_str());
    return 1;
}

static int albumArtwork(lua_State* L)
{
    lua_pushnil(L);
    //Tag* reader = checkReadWriter(L);
    //bool found = getAlbumArtwork(L, reader->tagFile);
    return 1;
}

/**
 * Frees the file object stored in the userdata
 */
static int free_tagReadWriter(lua_State* L)
{
    Tag* reader = checkReadWriter(L);
    free(reader->tagFile);
    free(reader->mimeType);
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
    {"artist",       artist},
    {"album",        album},
    {"title",        title},
    {"track",        track},
    {"year",         year},
    {"genre",        genre},
    {"length",       length},
    {"albumArtwork", albumArtwork},
    {"mimeType",     mimeType},
    {"close",        free_tagReadWriter},
    {"__gc",         garbage_collect_tagReadWriter},
    {NULL,           NULL}
};

LUALIB_API int luaopen_taglib(lua_State* L)
{
    /* Create the metatable for the userdata and assign function calls to it */
    luaL_newmetatable(L, TAG_READ_WRITE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, tag_methods, 0);
    luaL_newlib(L, tag_functions);
    return 1;
}
