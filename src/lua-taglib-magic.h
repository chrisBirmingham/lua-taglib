#ifndef LUA_TAGLIB_MAGIC
#define LUA_TAGLIB_MAGIC

#include <magic.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

namespace LuaTagLib
{
    namespace Magic
    {
        class FileTypeResolver
        {
            protected:
                magic_t magicCookie;

                std::string errorMessage;

                inline void closeMagic()
                {
                    if (this->magicCookie != NULL) {
                        magic_close(this->magicCookie);
                        this->magicCookie = NULL;
                    }
                }

            public:
                const std::string detectByFilePath(const std::string filePath);

                const std::string detectByBuffer(const char* buffer, size_t bufferLength);

                bool initialiseMagic();

                inline const std::string getErrorMessage() const
                {
                    return this->errorMessage;
                }

                FileTypeResolver() : magicCookie(NULL), errorMessage("") {}

                ~FileTypeResolver()
                {
                    this->closeMagic();
                }

        };
    }
}

#endif