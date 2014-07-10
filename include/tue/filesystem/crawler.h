#ifndef TUE_FILESYSTEM_CRAWLER_H_
#define TUE_FILESYSTEM_CRAWLER_H_

#include "path.h"

#include "boost/filesystem.hpp"

namespace tue
{
namespace filesystem
{

class Crawler
{

public:

    Crawler();

    Crawler(const Path& root_path);

    virtual ~Crawler();

    void setRootPath(const Path& path);

    void setRecursive(bool b = true) { recursive_ = b; }

    void setIgnoreHiddenDirectories(bool b = true) { ignore_hidden_dirs_ = b; }

    void setIgnoreHiddenFiles(bool b = true) { ignore_hidden_files_ = b; }

    void setListDirectories(bool b = true) { list_dirs_ = b; }

    void setListFiles(bool b = true) { list_files_ = b; }

    bool nextPath(Path& path);

private:

    bool recursive_, ignore_hidden_dirs_, ignore_hidden_files_, list_dirs_, list_files_;

    boost::filesystem::recursive_directory_iterator it_dir_;

};

}

}

#endif
