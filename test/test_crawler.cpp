#include <tue/filesystem/crawler.h>

#include <iostream>

int main(int argc, char **argv) {

    tue::filesystem::Crawler crawler(".");
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
