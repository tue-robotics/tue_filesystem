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

std::string Path::filename() const
{
    return boost::filesystem::path(path_).filename().string();
}

// ----------------------------------------------------------------------------------------------------

Path Path::parentPath() const
{
    std::string str = boost::filesystem::path(path_).parent_path().string();
    if (str.empty())
        str = ".";

    return Path(str);
}

// ----------------------------------------------------------------------------------------------------

bool Path::exists() const
{
    return boost::filesystem::exists(boost::filesystem::path(path_));
}

// ----------------------------------------------------------------------------------------------------

bool Path::isRegularFile() const
{
    return boost::filesystem::is_regular_file(boost::filesystem::path(path_));
}

// ----------------------------------------------------------------------------------------------------

bool Path::isDirectory() const
{
    return boost::filesystem::is_directory(boost::filesystem::path(path_));
}

// ----------------------------------------------------------------------------------------------------

std::time_t Path::lastWriteTime() const
{
    return boost::filesystem::last_write_time(boost::filesystem::path(path_));
}

// ----------------------------------------------------------------------------------------------------

Path& Path::removeExtension()
{
    std::string ext = extension();

    if (!ext.empty())
        path_ = path_.substr(0, path_.size() - ext.size());

    return *this;
}

// ----------------------------------------------------------------------------------------------------

Path Path::withoutExtension() const
{
    std::string ext = extension();

    Path p;
    if (!ext.empty())
        p.path_ = path_.substr(0, path_.size() - ext.size());
    else
        p = *this;

    return p;
}

// ----------------------------------------------------------------------------------------------------

Path Path::join(const Path& path) const
{
    Path p(*this);

    if (!p.path_.empty() && p.path_[p.path_.size() - 1] != '/')
        p.path_ += '/';

    p.path_ += path.path_;
    return p;
}

}

}
