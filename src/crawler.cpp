#include "tue/filesystem/crawler.h"

namespace tue
{
namespace filesystem
{

// ----------------------------------------------------------------------------------------------------

Crawler::Crawler() : recursive_(true), ignore_hidden_dirs_ (true), ignore_hidden_files_(true)
{
}

// ----------------------------------------------------------------------------------------------------

Crawler::Crawler(const Path& root_path) : recursive_(true), ignore_hidden_dirs_ (true), ignore_hidden_files_(true)
{
    setRootPath(root_path);
}

// ----------------------------------------------------------------------------------------------------

Crawler::~Crawler()
{
}

// ----------------------------------------------------------------------------------------------------

void Crawler::setRootPath(const Path& path)
{
    boost::filesystem::path p(path.string());
    it_dir_ = boost::filesystem::recursive_directory_iterator(p);
}

// ----------------------------------------------------------------------------------------------------

bool Crawler::nextPath(Path& path)
{
    boost::filesystem::recursive_directory_iterator end;

    while(it_dir_ != end)
    {
        bool found = false;

        if (boost::filesystem3::is_regular_file(*it_dir_))
        {
            if (!ignore_hidden_files_ || it_dir_->path().filename().string()[0] != '.')
            {
                path = it_dir_->path().string();
                found = true;
            }
        }
        else if (ignore_hidden_dirs_ && boost::filesystem::is_directory(*it_dir_) && it_dir_->path().filename().string()[0] == '.')
        {
            it_dir_.no_push();
        }

        try
        {
            ++it_dir_;
        }
        catch(std::exception& ex)
        {
            // We couldn't access the next item in the collection, so we assume it refers to a directory that we can't
            // access and we ask the iterator class not to navigate in that directory but skip to the next element.

            std::cout << ex.what() << std::endl;
            it_dir_.no_push();
        }

        if (found)
            return true;
    }

    return false;
}

}

}
