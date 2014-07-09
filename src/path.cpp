#include "tue/filesystem/path.h"

#include "boost/filesystem.hpp"

namespace tue
{
namespace filesystem
{

// ----------------------------------------------------------------------------------------------------

Path::Path()
{
}

// ----------------------------------------------------------------------------------------------------

Path::Path(const std::string& path) : path_(path) {}

// ----------------------------------------------------------------------------------------------------

Path::~Path()
{
}

// ----------------------------------------------------------------------------------------------------

std::string Path::extension() const
{
    return boost::filesystem::path(path_).extension().string();
}

// ----------------------------------------------------------------------------------------------------

Path& Path::removeExtension()
{
    std::string ext = extension();

    if (!ext.empty())
    {
        path_ = path_.substr(0, path_.size() - ext.size());
    }

    return *this;
}

// ----------------------------------------------------------------------------------------------------

Path& Path::join(const Path& path)
{
    if (!path_.empty() && path_[path_.size() - 1] != '/')
        path_ += '/';

    path_ += path.path_;
    return *this;
}

}

}
