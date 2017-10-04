/** @file crawler.cpp Implementation of the file system crawler. */

#include "tue/filesystem/crawler.h"
#include <iostream>

namespace tue {
namespace filesystem {

/**
 * Default constructor of the crawler iterator class.
 * Creates an iterator that returns all non-hidden files in all non-hidden (sub-)directories.
 * @note Root path must be set usng #setRootPath before usage.
 */
Crawler::Crawler() : recursive_(true), ignore_hidden_dirs_(true), ignore_hidden_files_(true),
        list_dirs_(false), list_files_(true)
{
}

/**
 * Constructor of the crawler iterator with a root path.
 * Creates an iterator that returns all non-hidden files in all non-hidden (sub-)directories.
 * @param root_path Starting point of the sub-tree to expand.
 */
Crawler::Crawler(const Path& root_path) : recursive_(true), ignore_hidden_dirs_(true),
        ignore_hidden_files_(true), list_dirs_(false), list_files_(true)
{
    setRootPath(root_path);
}

/** Desctructor. */
Crawler::~Crawler()
{
}

/**
 * Sets (or change) the root path of the crawler instance.
 * Also resets the iteration.
 * @param root_path New root path to use for the iteration.
 */
void Crawler::setRootPath(const Path& root_path)
{
    boost::filesystem::path p(root_path.string());
    it_dir_ = boost::filesystem::recursive_directory_iterator(p);
}

/**
 * Get the next path at the file system of the crawl.
 * @param path [out] Next found path at the file system, if the function returns successfully.
 * @return Whether a next path was found, otherwise the crawl is finished.
 */
bool Crawler::nextPath(Path& path)
{
    boost::filesystem::recursive_directory_iterator end;

    while(it_dir_ != end) {
        // Compute whether the current name should be returned to the caller.
        bool found = false;
        if (list_files_ && boost::filesystem::is_regular_file(*it_dir_)) {
            if (!ignore_hidden_files_ || it_dir_->path().filename().string()[0] != '.') {
                path = it_dir_->path().string();
                found = true;
            }
        } else if (boost::filesystem::is_directory(*it_dir_)) {
            if (!ignore_hidden_dirs_ || it_dir_->path().filename().string()[0] != '.') {
                if (list_dirs_) {
                    path = it_dir_->path().string();
                    found = true;
                }

                if (!recursive_) it_dir_.no_push();

            } else {
                it_dir_.no_push();
            }
        }

        // Advance to the next file or directory.
        try {
            ++it_dir_;

        } catch(std::exception& ex) {
            // We couldn't access the next item in the collection, so we assume it refers to a directory that we can't
            // access and we ask the iterator class not to navigate in that directory but skip to the next element.
            std::cout << ex.what() << std::endl;
            it_dir_.no_push();
        }

        if (found) return true;
    }

    return false;
}

} // end filesystem namespace
} // end tue namespace
