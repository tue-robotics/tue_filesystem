#ifndef TUE_FILESYSTEM_CRAWLER_H_
#define TUE_FILESYSTEM_CRAWLER_H_

/** @file crawler.h File system crawler definition. */

#include "path.h"
#include "boost/filesystem.hpp"

namespace tue
{
namespace filesystem
{

/**
 * File system crawler, recursively walk over all files and directories in
 * a tree from the provided root path.
 */
class Crawler
{
public:
    Crawler();
    Crawler(const Path& root_path);
    virtual ~Crawler();

    void setRootPath(const Path& path);

    /**
     * Enable or disable iterating over sub-directories.
     * @param b Whether to expand sub-directories too.
     */
    void setRecursive(bool b = true) { recursive_ = b; }

    /**
     * Enable or disable iterating over hidden directories.
     * @param b Whether to skip hidden directories.
     */
    void setIgnoreHiddenDirectories(bool b = true) { ignore_hidden_dirs_ = b; }

    /**
     * Enable or disable iterating over hidden files.
     * @param b Whether to skip hidden files.
     */
    void setIgnoreHiddenFiles(bool b = true) { ignore_hidden_files_ = b; }

    /**
     * Enable or disable returning directories in the iterator.
     * @param b Whether directories should be returned in the iterator.
     */
    void setListDirectories(bool b = true) { list_dirs_ = b; }

    /**
     * Enable or disable returning files in the iterator.
     * @param b Whether files should be returned in the iterator.
     */
    void setListFiles(bool b = true) { list_files_ = b; }

    bool nextPath(Path& path);

private:
    bool recursive_;           ///< If set, iterator also returns content of sub-directories.
    bool ignore_hidden_dirs_;  ///< If set, hidden directories are not expanded.
    bool ignore_hidden_files_; ///< If set, hidden files are not returned in the iterator.
    bool list_dirs_;           ///< If set, iterator returns found directories.
    bool list_files_;          ///< If set, iterator returns found files.

    /** Internal file system iterator. */
    boost::filesystem::recursive_directory_iterator it_dir_;
};

} // end filesystem namespace
} // end tue namespace

#endif
