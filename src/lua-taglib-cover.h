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
#include <taglib/mp4tag.h>
#include <taglib/textidentificationframe.h>
#include <taglib/tstring.h>
#include <taglib/vorbisfile.h>
#include <taglib/tbytevector.h>
#include <string>
#include "lua-taglib.h"

namespace LuaTagLib {

    namespace Cover {

        class Artwork {
            protected:
                TagLib::ByteVector* data;

                std::string errorMessage;

                bool extractAPE(const TagLib::APE::Tag* tag);

                bool extractASF(const TagLib::ASF::File* asfFile);

                bool extractFLAC(TagLib::FLAC::File* flacFile);

                bool extractMP4(const TagLib::MP4::File* mp4File);

                bool extractID3(const TagLib::ID3v2::Tag* tag);

                bool supportsAlbumArt(std::string mimetype);

            public:
                Artwork() : data(NULL), errorMessage("") {}

                inline const TagLib::ByteVector* getData() const
                {
                    return this->data;
                }

                inline const std::string getErrorMessage() const
                {
                    return this->errorMessage;
                }

                bool getAlbumArtwork(TagLib::File* file, std::string mime);

                ~Artwork()
                {
                    free(this->data);
                }
        };
    }
}

#endif
