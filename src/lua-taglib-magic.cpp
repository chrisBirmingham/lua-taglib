#include "lua-taglib-magic.h"

using namespace LuaTagLib;

const std::string Magic::FileTypeResolver::detectByFilePath(const std::string filePath)
{
    const char* mime;
    if ((mime = magic_file(this->magicCookie, filePath.c_str())) == NULL) {
        const char* error = magic_error(this->magicCookie);
        this->errorMessage = std::string(error, strlen(error));
        return "";
    }

    return std::string(mime, strlen(mime));
}

const std::string Magic::FileTypeResolver::detectByBuffer(const char* buffer, size_t bufferLength)
{
    const char* mime;
    if ((mime = magic_buffer(this->magicCookie, buffer, bufferLength)) == NULL) {
        const char* error = magic_error(this->magicCookie);
        this->errorMessage = std::string(error, strlen(error));
        return "";
    }

    return std::string(mime, strlen(mime));
}

bool Magic::FileTypeResolver::initialiseMagic()
{
    if ((this->magicCookie = magic_open(MAGIC_MIME_TYPE | MAGIC_ERROR)) == NULL) {
        const char* error = magic_error(this->magicCookie);
        this->errorMessage = std::string(error, strlen(error));
        return false;
    }

    if (magic_load(this->magicCookie, NULL) != 0) {
        const char* error = magic_error(this->magicCookie);
        this->errorMessage = std::string(error, strlen(error));
        this->closeMagic();
        return false;
    }

    return true;
}