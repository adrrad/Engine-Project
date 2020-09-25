#include "platform/io/File.hpp"

#include "utilities/StringUtilities.hpp"


#include <iostream>
#include <fstream>
namespace Engine::Platform::IO
{

FileSize File::GetSize(Path path)
{
    std::ifstream file(path.ToString(), std::ios::in | std::ios::binary);
    std::string str(std::istreambuf_iterator<char>{file}, {});
    return FileSize(str.size());
}
    
File::File(std::string absolutePath) :
    FilePath(Path(absolutePath)), 
    Extension(Utilities::SplitOnFirst(absolutePath, ".")),
    Size(GetSize(Path(absolutePath)))
{

}

File::~File()
{

}

void File::Write(char* data)
{
    std::ofstream file;
    file.open(FilePath.ToString());
    file << data;
    file.close();
}

char* File::ReadAll()
{
    std::ifstream file(FilePath.ToString(), std::ios::in | std::ios::binary);
    std::string str(std::istreambuf_iterator<char>{file}, {});
    char* data = new char[str.size()];
    memcpy(data, str.c_str(), str.size());
    return data;
}


} // namespace Engine::Platform::IO

