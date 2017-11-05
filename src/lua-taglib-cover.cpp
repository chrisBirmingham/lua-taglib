#include "lua-taglib-cover.h"

using namespace LuaTagLib;

const std::string NO_ALBUM_DATA = "Audio file does not contain any album artwork";
const std::string NOT_SUPPORTED = "The audio file provided does not support the storage of album artwork";

bool Cover::Artwork::extractAPE(const TagLib::APE::Tag* tag)
{
    const TagLib::APE::ItemListMap& listMap = tag->itemListMap();
    if (listMap.contains("COVER ART (FRONT)")) {
        TagLib::ByteVector nullStringTerminator(1, 0);
        TagLib::ByteVector item = listMap["COVER ART (FRONT)"].value();
        int pos;
        if ((pos = item.find(nullStringTerminator)) != -1) {	// Skip the filename.
            const TagLib::ByteVector& pic = item.mid(pos + 1);
            data = new TagLib::ByteVector(pic.data(), pic.size());
            return true;
        }
    }

    return false;
}

bool Cover::Artwork::extractASF(const TagLib::ASF::File* asfFile)
{
    const TagLib::ASF::AttributeListMap& attrListMap = asfFile->tag()->attributeListMap();
    if (attrListMap.contains("WM/Picture")) {
        const TagLib::ASF::AttributeList& attrList = attrListMap["WM/Picture"];
        if (!attrList.isEmpty()) {
            for (int i = 0; i < attrList.size(); i++) {
                const TagLib::ASF::Picture& wmpic = attrList[i].toPicture();
                if (wmpic.isValid()) {
                    this->data = new TagLib::ByteVector(wmpic.picture().data(), wmpic.picture().size());
                    return true;
                }
            }
        }
    }

    return false;
}

bool Cover::Artwork::extractFLAC(TagLib::FLAC::File* flacFile)
{
    const TagLib::List<TagLib::FLAC::Picture*>& picList = flacFile->pictureList();
    if (!picList.isEmpty()) {
        this->data = new TagLib::ByteVector(picList[0]->data().data(), picList[0]->data().size());
        return true;
    }

    return false;
}

bool Cover::Artwork::extractMP4(const TagLib::MP4::File* mp4File)
{
    //if (mp4File->hasMP4Tag()) {
        TagLib::MP4::Tag* tag = mp4File->tag();
        const TagLib::MP4::ItemListMap& itemListMap = tag->itemListMap();
        if (itemListMap.contains("covr")) {
            const TagLib::MP4::CoverArtList& coverArtList = itemListMap["covr"].toCoverArtList();
            if (!coverArtList.isEmpty()) {
                this->data = new TagLib::ByteVector(coverArtList.front().data().data(), coverArtList.front().data().size());
                return true;
            }
        }
    //}

    return false;
}

bool Cover::Artwork::extractID3(const TagLib::ID3v2::Tag* tag)
{
    const TagLib::ID3v2::FrameList& frameList = tag->frameList("APIC");
    if (!frameList.isEmpty()) {
        TagLib::ID3v2::AttachedPictureFrame* frame = (TagLib::ID3v2::AttachedPictureFrame*) frameList.front();
        this->data = new TagLib::ByteVector(frame->picture().data(), frame->picture().size());
        return true;
    }

	return false;
}

bool Cover::Artwork::supportsAlbumArt(std::string mimetype)
{
    if (mimetype == "audio/mpeg") {
        return true;
    } else if (mimetype == "audio/flac") {
        return true;
    } else if (mimetype == "audio/mp4") {
        return true;
    } else if (mimetype == "audio/x-ms-wma") {
        return true;
    } else if (mimetype == "audio/x-monkeys-audio") {
        return true;
    } else if (mimetype == "audio/x-musepack") {
        return true;
    }

    return false;
}

/**
 * Attempts to retrieve the album artwork stored inside an audio file.
 * Uses static casting as the object is of the desired type
 */
bool Cover::Artwork::getAlbumArtwork(TagLib::File* file, std::string mime)
{
    // Check if the mime type supports album artwork
    if (!this->supportsAlbumArt(mime)) {
        this->errorMessage = NOT_SUPPORTED;
        return false;
    }

    bool found = false;

    if (mime == "audio/mpeg") {
        TagLib::MPEG::File* f = static_cast<TagLib::MPEG::File*>(file);
        if (f->hasID3v2Tag()) {
            found = this->extractID3(f->ID3v2Tag());
        }

        if (!found && f->hasAPETag()) {
            found = this->extractAPE(f->APETag());
        }

    } else if (mime == "audio/flac") {
        TagLib::FLAC::File* f = static_cast<TagLib::FLAC::File*>(file);
        found = this->extractFLAC(f);

        if (!found && f->hasID3v2Tag()) {
            found = this->extractID3(f->ID3v2Tag());
        }
    } else if (mime == "audio/mp4") {
        TagLib::MP4::File* f = static_cast<TagLib::MP4::File*>(file);
        found = this->extractMP4(f);
    } else if (mime == "audio/x-ms-wma") {
        TagLib::ASF::File* f = static_cast<TagLib::ASF::File*>(file);
        found = this->extractASF(f);
    } else if (mime == "audio/x-monkeys-audio") {
        TagLib::APE::File* f = static_cast<TagLib::APE::File*>(file);
        if (f->hasAPETag()) {
            found = this->extractAPE(f->APETag());
        }
    } else if (mime == "audio/x-musepack") {
        TagLib::MPC::File* f = static_cast<TagLib::MPC::File*>(file);
        if (f->hasAPETag()) {
            found = this->extractAPE(f->APETag());
        }
    }

    // If we hit here we have no album data, return with that info
    if (!found) {
        this->errorMessage = NO_ALBUM_DATA;
        return false;
    }
    return true;
}