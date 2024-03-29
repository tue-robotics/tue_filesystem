/** @file crawler.cpp Implementation of the file system crawler. */

#include "tue/filesystem/crawler.h"

#include <console_bridge/console.h>

namespace tue
{
namespace filesystem
{

Crawler::Crawler() : recursive_(true), ignore_hidden_dirs_(true), ignore_hidden_files_(true),
        list_dirs_(false), list_files_(true)
{
}

Crawler::Crawler(const Path& root_path) : recursive_(true), ignore_hidden_dirs_(true),
        ignore_hidden_files_(true), list_dirs_(false), list_files_(true)
{
    setRootPath(root_path);
}

Crawler::~Crawler()
{
}

void Crawler::setRootPath(const Path& root_path)
{
    boost::filesystem::path p(root_path.string());
    it_dir_ = boost::filesystem::recursive_directory_iterator(p);
}

bool Crawler::nextPath(Path& path)
{
    boost::filesystem::recursive_directory_iterator end;

    while(it_dir_ != end)
    {
        // Compute whether the current name should be returned to the caller.
        bool found = false;

        if (list_files_ && boost::filesystem::is_regular_file(*it_dir_))
        {
            if (!ignore_hidden_files_ || it_dir_->path().filename().string()[0] != '.')
            {
                path = it_dir_->path().string();
                found = true;
            }
        }
        else if (boost::filesystem::is_directory(*it_dir_))
        {
            if (!ignore_hidden_dirs_ || it_dir_->path().filename().string()[0] != '.')
            {
                if (list_dirs_)
                {
                    path = it_dir_->path().string();
                    found = true;
                }

                if (!recursive_)
                    it_dir_.no_push();
            }
            else
            {
                it_dir_.no_push();
            }
        }

        // Advance to the next file or directory.
        try
        {
            ++it_dir_;
        }
        catch(std::exception& ex)
        {
            // We couldn't access the next item in the collection, so we assume it refers to a directory that we can't
            // access and we ask the iterator class not to navigate in that directory but skip to the next element.

            CONSOLE_BRIDGE_logError(ex.what());
            it_dir_.no_push();
        }

        if (found)
            return true;
    }

    return false;
}

} // end filesystem namespace
} // end tue namespace
