#include <tue/filesystem/crawler.h>

#include <iostream>

int main(int argc, char** argv)
{
    std::string path(".");
    if (argc > 1)
        // Take the last provided path
        path = argv[argc-1];

    tue::filesystem::Crawler crawler(path);
    crawler.setListDirectories();

    tue::filesystem::Path p;
    while(crawler.nextPath(p))
    {
        if (p.isDirectory())
            std::cout << "DIR:  " << p << std::endl;
        else if (p.isRegularFile())
            std::cout << "FILE: " << p << std::endl;
    }

    return 0;
}
