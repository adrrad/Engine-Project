#include "platform/io/File.hpp"

#include "utilities/StringUtilities.hpp"

#include <iostream>
#include <fstream>
namespace Engine::Platform::IO
{

ResourceType File::GetResourceTypeFromExtension(std::string extension)
{
    if(extension == ".txt") return ResourceType::TEXT;
    if(extension == ".json") return ResourceType::JSON;
    return ResourceType::OTHER;
}
    
File::File(std::string absolutePath,  char* data, FileSize size) : 
    Data(Utilities::Array(size, data)), 
    Size(size),
    Path(absolutePath), 
    Extension(Utilities::SplitOnFirst(absolutePath, ".")),
    Type(GetResourceTypeFromExtension(Extension))
{

}

File::~File()
{

}

void File::Write()
{
    std::ofstream file;
    file.open(Path);
    char* data = Data.Data();
    file << data;
    file.close();
}

File* Open(std::string absolutePath)
{
    std::ifstream file;
    file.open(absolutePath);
    file.seekg(0, std::ios::end);
    FileSize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    Utilities::Array<char> data(fileSize);
    file.read(data.Data(), fileSize);
    return new File(absolutePath, data.Data(), fileSize);    
}

} // namespace Engine::Platform::IO

