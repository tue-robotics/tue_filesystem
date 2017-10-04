/** @file path.cpp Implementation of the path class. */

#include "tue/filesystem/path.h"
#include "boost/filesystem.hpp"

namespace tue {
namespace filesystem {

/** Default constructor, without setting a valid path. */
Path::Path()
{
}

/**
 * Regular constructor.
 * @param path Path to set.
 */
Path::Path(const std::string& path) : path_(path) {}

/** Destructor. */
Path::~Path()
{
}

/**
 * Get the last extension of the path if available.
 * @return The last extensionof the path including the leading dot, or the empty string.
 */
std::string Path::extension() const
{
    return boost::filesystem::path(path_).extension().string();
}

/**
 * Return the part of the path after the last directory separator.
 * @return The name part after the last directory separator, if it exists.
 *      If the path represents the file system root (\c "/"), the path itself is returned, else \c "." is returned.
 */
std::string Path::filename() const
{
    return boost::filesystem::path(path_).filename().string();
}

/**
 * Get the parent directory of the path.
 * @return The empty string is the path is the file system root (\c "/"), else the path without the #filename()
 *      part and last directory separator.
 */
Path Path::parentPath() const
{
    std::string str = boost::filesystem::path(path_).parent_path().string();
    if (str.empty()) {
        str = ".";
    }

    return Path(str);
}

/**
 * Verify whether the path actually exists at the file system.
 * @return Whether the path represents an existing path at the file system at the time of the call.
 */
bool Path::exists() const
{
    return boost::filesystem::exists(boost::filesystem::path(path_));
}

/**
 * Test whether the path is a regular file.
 * @return Whether the path is a regular file.
 * @note Getting \c false does not imply the path is a directory.
 */
bool Path::isRegularFile() const
{
    return boost::filesystem::is_regular_file(boost::filesystem::path(path_));
}

/**
 * Test whether the path is a directory.
 * @return Whether the path is a directory.
 * @note Getting \c false does not imply the path is a regular file.
 */
bool Path::isDirectory() const
{
    return boost::filesystem::is_directory(boost::filesystem::path(path_));
}

/**
 * Get the last modification time of the path at the file system as reported by the OS.
 * For directories, this may not include last modification time of its content.
 * @return Last modification time of the path.
 */
std::time_t Path::lastWriteTime() const
{
    return boost::filesystem::last_write_time(boost::filesystem::path(path_));
}

/**
 * Remove the extension from the path.
 * @return The object after removal of the last extension.
 */
Path& Path::removeExtension()
{
    std::string ext = extension();

    if (!ext.empty()) {
        path_ = path_.substr(0, path_.size() - ext.size());
    }

    return *this;
}

/**
 * Construct a new path from this path while removing the extension from it.
 * @return Same path as the object, but without last extension.
 */
Path Path::withoutExtension() const
{
    std::string ext = extension();

    Path p;
    if (!ext.empty()) {
        p.path_ = path_.substr(0, path_.size() - ext.size());
    } else {
        p = *this;
    }

    return p;
}

/**
 * Construct a new path by appending the given \a sub_path to this path.
 * @param sub_path Path to append to this path.
 * @return New path consisting of this path, appended with \a sub_path.
 */
Path Path::join(const Path& sub_path) const
{
    Path p(*this);

    if (!p.path_.empty() && p.path_[p.path_.size() - 1] != '/') {
        p.path_ += '/';
    }

    p.path_ += sub_path.path_;
    return p;
}

} // end filesystem namespace
} // end tue namespace
