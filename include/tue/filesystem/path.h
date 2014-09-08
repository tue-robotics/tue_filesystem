#ifndef TUE_FILESYSTEM_PATH_H_
#define TUE_FILESYSTEM_PATH_H_

#include <string>
#include <ctime>

namespace tue
{
namespace filesystem
{

class Path
{

public:

    Path();

    Path(const char* path) : path_(path) {}

    Path(const std::string& path);

    virtual ~Path();

    const std::string& string() const { return path_; }

    std::string extension() const;

    Path parentPath() const;

    bool exists() const;

    bool isRegularFile() const;

    bool isDirectory() const;

    std::time_t lastWriteTime() const;

    Path& removeExtension();

    Path withoutExtension() const;

    Path join(const Path& path) const;

    std::string filename() const;

    friend std::ostream& operator<< (std::ostream& out, const Path& p) {
        out << p.path_;
        return out;
    }

private:

    std::string path_;


};

}

}

#endif
