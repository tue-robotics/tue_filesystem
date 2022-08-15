#ifndef TUE_FILESYSTEM_PATH_H_
#define TUE_FILESYSTEM_PATH_H_

/** @file path.h File system path class. */

#include <string>
#include <ctime>

namespace tue
{
namespace filesystem
{

/**
 * Path class for a file system path, providing several useful functions for querying
 * and manipulating a path.
 */
class Path
{
public:
//    /** Default constructor, without setting a valid path. */
    Path();

//    /**
//     * Regular constructor
//     * @param path Path to set.
//     */
    Path(const std::string& path);

    /**
     * Overloaded constructor for `char*` paths.
     * @param path Path to set in the object.
     */
    Path(const char* path) : path_(path)
    {
    }

    /** Destructor */
    virtual ~Path();

    /**
     * Get the complete path.
     * @return The entire path, as stored in in the object.
     */
    const std::string& string() const
    {
        return path_;
    }

    /**
     * Get the last extension of the path if available.
     * @return The last extension of the path including the leading dot, or the empty string.
     */
    std::string extension() const;

    /**
     * Return the part of the path after the last directory separator.
     * @return The name part after the last directory separator, if it exists.
     *      If the path represents the file system root (\c "/"), the path itself is returned, else \c "." is returned.
     */
    std::string filename() const;

    /**
     * Get the parent directory of the path.
     * @return The empty string is the path is the file system root (\c "/"), else the path without the #filename()
     *      part and last directory separator.
     */
    Path parentPath() const;

    /**
     * Verify whether the path actually exists at the file system.
     * @return Whether the path represents an existing path at the file system at the time of the call.
     */
    bool exists() const;

    /**
     * Test whether the path is a regular file.
     * @return Whether the path is a regular file.
     * @note Getting \c false does not imply the path is a directory.
     */
    bool isRegularFile() const;

    /**
     * Test whether the path is a directory.
     * @return Whether the path is a directory.
     * @note Getting \c false does not imply the path is a regular file.
     */
    bool isDirectory() const;

    /**
     * Get the last modification time of the path at the file system as reported by the OS.
     * For directories, this may not include last modification time of its content.
     * @return Last modification time of the path.
     */
    std::time_t lastWriteTime() const;

    /**
     * Remove the extension from the path.
     * @return The object after removal of the last extension.
     */
    Path& removeExtension();

    /**
     * Construct a new path from this path while removing the extension from it.
     * @return Same path as the object, but without last extension.
     */
    Path withoutExtension() const;

    /**
     * Construct a new path by appending the given \a sub_path to this path.
     * @param path Path to append to this path.
     * @return New path consisting of this path, appended with \a sub_path.
     */
    Path join(const Path& path) const;

//    /** Support printing the path. */
    friend std::ostream& operator<< (std::ostream& out, const Path& p)
    {
        out << p.path_;
        return out;
    }

private:
    std::string path_; ///< Text of the file system path.
};

} // end filesystem namespace
} // end tue namespace

#endif
