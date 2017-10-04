#ifndef TUE_FILESYSTEM_PATH_H_
#define TUE_FILESYSTEM_PATH_H_

/** @file path.h File system path class. */

#include <string>
#include <ctime>

namespace tue {
namespace filesystem {

/**
 * Path class for a file system path, providing several useful functions for querying
 * and manipulating a path.
 */
class Path
{
public:
    Path();
    Path(const std::string& path);

    /**
     * Overloaded constructor for \code char* paths.
     * @param path Path to set in the object.
     */
    Path(const char* path) : path_(path)
    {
    }

    virtual ~Path();

    /**
     * Get the complete path.
     * @return The entire path, as stored in in the object.
     */
    const std::string& string() const
    {
        return path_;
    }

    std::string extension() const;
    std::string filename() const;
    Path parentPath() const;

    bool exists() const;
    bool isRegularFile() const;
    bool isDirectory() const;
    std::time_t lastWriteTime() const;

    Path& removeExtension();
    Path withoutExtension() const;
    Path join(const Path& path) const;

    /** Support printing the path. */
    friend std::ostream& operator<< (std::ostream& out, const Path& p) {
        out << p.path_;
        return out;
    }

private:
    std::string path_; ///< Text of the file system path.
};

} // end filesystem namespace
} // end tue namespace

#endif
