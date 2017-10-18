#ifndef LUA_TAGLIB_COVER
#define LUA_TAGLIB_COVER

#include <lua.hpp>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/apefile.h>
#include <taglib/apetag.h>
#include <taglib/asffile.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/commentsframe.h>
#include <taglib/flacfile.h>
#include <taglib/id3v1genres.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpcfile.h>
#include <taglib/mpegfile.h>
#include <taglib/mp4file.h>
#include <taglib/textidentificationframe.h>
#include <taglib/tstring.h>
#include <taglib/vorbisfile.h>
#include <taglib/tbytevector.h>
#include <string>
#include "lua-taglib.h"

#define TAG_ALBUM_ART "TAG_ALBUM_ART"

struct tagAlbumArtwork {
    TagLib::ByteVector* data;
    std::string* mimeType;
};

typedef tagAlbumArtwork TagAlbumArtwork;
static TagLib::ByteVector* extractAPE(TagLib::APE::Tag* tag);

static TagLib::ByteVector* extractASF(TagLib::ASF::File* file);

static TagLib::ByteVector* extractFLAC(TagLib::FLAC::File* file);

static TagLib::ByteVector* extractMP4(TagLib::MP4::File* file);

static TagLib::ByteVector* extractID3(TagLib::ID3v2::Tag* tag);

static bool supportsAlbumArt(const char* mimeType);

TagLib::ByteVector* getAlbumArtwork(lua_State* L, TagLib::File* fr, const char* mimeType);

TagAlbumArtwork* createAlbumArtwork(lua_State* L);

static int data(lua_State* L);

static int mimeType(lua_State* L);

static int size(lua_State* L);

extern const luaL_Reg album_methods[];

#endif
