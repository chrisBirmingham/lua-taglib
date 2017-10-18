#include "lua-taglib-cover.h"

const char* NO_ALBUM_DATA = "Audio file does not contain any album artwork";
const char* NOT_SUPPORTED = "The audio file provided does not support the storage of album artwork";

static TagLib::ByteVector* extractAPE(TagLib::APE::Tag* tag)
{
    const TagLib::APE::ItemListMap& listMap = tag->itemListMap();
    if (listMap.contains("COVER ART (FRONT)")) {
        TagLib::ByteVector nullStringTerminator(1, 0);
        TagLib::ByteVector item = listMap["COVER ART (FRONT)"].value();
        int pos;
        if ((pos = item.find(nullStringTerminator)) != -1) {	// Skip the filename.
            const TagLib::ByteVector& pic = item.mid(pos + 1);
            return new TagLib::ByteVector(pic.data(), pic.size());
        }
    }

    return NULL;
}

static TagLib::ByteVector* extractASF(TagLib::ASF::File* file)
{
    const TagLib::ASF::AttributeListMap& attrListMap = file->tag()->attributeListMap();
    if (attrListMap.contains("WM/Picture")) {
        const TagLib::ASF::AttributeList& attrList = attrListMap["WM/Picture"];
        if (!attrList.isEmpty()) {
            for (int i = 0; i < attrList.size(); i++) {
                const TagLib::ASF::Picture& wmpic = attrList[i].toPicture();
                if (wmpic.isValid()) {
                    return new TagLib::ByteVector(wmpic.picture().data(), wmpic.picture().size());
                }
            }
        }
    }

    return NULL;
}

static TagLib::ByteVector* extractFLAC(TagLib::FLAC::File* file)
{
    const TagLib::List<TagLib::FLAC::Picture*>& picList = file->pictureList();
    if (!picList.isEmpty()) {
        return new TagLib::ByteVector(picList[0]->data().data(), picList[0]->data().size());
    }

    return NULL;
}

static TagLib::ByteVector* extractMP4(TagLib::MP4::File* file)
{
    TagLib::MP4::Tag* tag = file->tag();
    const TagLib::MP4::ItemListMap& itemListMap = tag->itemListMap();
    if (itemListMap.contains("covr")) {
        const TagLib::MP4::CoverArtList& coverArtList = itemListMap["covr"].toCoverArtList();
        if (!coverArtList.isEmpty()) {
            return new TagLib::ByteVector(coverArtList.front().data().data(), coverArtList.front().data().size());
        }
    }

    return NULL;
}

static TagLib::ByteVector* extractID3(TagLib::ID3v2::Tag* tag)
{
	const TagLib::ID3v2::FrameList& frameList = tag->frameList("APIC");
	if (!frameList.isEmpty()) {
        TagLib::ID3v2::AttachedPictureFrame* frame = (TagLib::ID3v2::AttachedPictureFrame*) frameList.front();
        return new TagLib::ByteVector(frame->picture().data(), frame->picture().size());
	}

	return NULL;
}

static bool supportsAlbumArt(const char* mimeType)
{
    if (strcmp(mimeType, "audio/mpeg") == 0) {
        return true;
    } else if (strcmp(mimeType, "audio/flac") == 0) {
        return true;
    } else if (strcmp(mimeType, "audio/mp4") == 0) {
        return true;
    } else if (strcmp(mimeType, "audio/x-ms-wma") == 0) {
        return true;
    } else if (strcmp(mimeType, "audio/x-monkeys-audio") == 0) {
        return true;
    } else if (strcmp(mimeType, "audio/x-musepack") == 0) {
        return true;
    }

    return false;
}

TagLib::ByteVector* getAlbumArtwork(lua_State* L, TagLib::File* fr, const char* mimeType)
{
    // Check if the mime type supports album artwork
    if (!supportsAlbumArt(mimeType)) {
        // if not return with error message
        lua_pushnil(L);
        lua_pushstring(L, NOT_SUPPORTED);
        return NULL;
    }

    TagLib::ByteVector* tmp;
    if (strcmp(mimeType, "audio/mpeg") == 0) {
        TagLib::MPEG::File* f = static_cast<TagLib::MPEG::File*>(fr);
        if (f->ID3v2Tag() && (tmp = extractID3(f->ID3v2Tag())) != NULL) {
            return tmp;
        }

        if (f->APETag() && (tmp = extractAPE(f->APETag())) != NULL) {
            return tmp;
        }

    } else if (strcmp(mimeType, "audio/flac") == 0) {
        TagLib::FLAC::File* f = static_cast<TagLib::FLAC::File*>(fr);

        if ((tmp = extractFLAC(f)) != NULL) {
            return tmp;
        }

        if (f->ID3v2Tag() && (tmp = extractID3(f->ID3v2Tag())) != NULL) {
            return tmp;
        }
    } else if (strcmp(mimeType, "audio/mp4") == 0) {
        TagLib::MP4::File* f = static_cast<TagLib::MP4::File*>(fr);
        if ((tmp = extractMP4(f))) {
            return tmp;
        }
    } else if (strcmp(mimeType, "audio/x-ms-wma") == 0) {
        TagLib::ASF::File* f = static_cast<TagLib::ASF::File*>(fr);
        if ((tmp = extractASF(f)) != NULL) {
            return tmp;
        }
    } else if (strcmp(mimeType, "audio/x-monkeys-audio") == 0) {
        TagLib::APE::File* f = static_cast<TagLib::APE::File*>(fr);
        if (f->APETag() && (tmp = extractAPE(f->APETag())) != NULL) {
            return tmp;
        }
    } else if (strcmp(mimeType, "audio/x-musepack") == 0) {
        TagLib::MPC::File* f = static_cast<TagLib::MPC::File*>(fr);
        if (f->APETag() && (tmp = extractAPE(f->APETag())) != NULL) {
            return tmp;
        }
    }

    // If we hit here we have no album data, return with that info
    lua_pushnil(L);
    lua_pushstring(L, NO_ALBUM_DATA);
    return NULL;
}

/**
 * Checks whether the first argument to a function of the userdata TAG_READ_WRITE
 */
static TagAlbumArtwork* checkAlbumArtwork(lua_State* L)
{
    void* artwork = luaL_checkudata(L, 1, TAG_ALBUM_ART);
    luaL_argcheck(L, artwork != NULL, 1, "`album class' expected");
    return (TagAlbumArtwork*)artwork;
}

/**
 * Creates a tag reader userdata
 */
TagAlbumArtwork* createAlbumArtwork(lua_State* L)
{
    TagAlbumArtwork* album = (TagAlbumArtwork*)lua_newuserdata(L, sizeof(TagAlbumArtwork) + sizeof(TagLib::ByteVector));
    luaL_getmetatable(L, TAG_ALBUM_ART);
    lua_setmetatable(L, -2);
    return album;
}

static int data(lua_State* L){
    TagAlbumArtwork* tagAlbum = checkAlbumArtwork(L);
    lua_pushlstring(L, tagAlbum->data->data(), tagAlbum->data->size());
    return 1;
}

static int mimeType(lua_State* L)
{
    TagAlbumArtwork* tagAlbum = checkAlbumArtwork(L);
    lua_pushstring(L, tagAlbum->mimeType->c_str());
    return 1;
}

static int size(lua_State* L)
{
    TagAlbumArtwork* tagAlbum = checkAlbumArtwork(L);
    lua_pushinteger(L, tagAlbum->data->size());
    return 1;
}

const luaL_Reg album_methods[] = {
    {"data",     data},
    {"mimeType", mimeType},
    {"size",     size},
    {NULL,       NULL}
};