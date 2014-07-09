#include <tue/filesystem/crawler.h>

int main(int argc, char **argv) {

    tue::filesystem::Crawler crawler(".");

    tue::filesystem::Path p;
    while(crawler.nextPath(p))
    {
        std::cout << p << std::endl;
        std::cout << "    " << p.extension() << std::endl;
        std::cout << "    " << p.removeExtension().join("poep") << std::endl;
    }

    return 0;
}
