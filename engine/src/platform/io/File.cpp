#include "platform/io/File.hpp"

#include "utilities/StringUtilities.hpp"

#include <iostream>
#include <fstream>
namespace Engine::Platform::IO
{

ResourceType File::GetResourceTypeFromExtension(std::string extension)
{
    if(extension == "txt") return ResourceType::TEXT;
    if(extension == "json") return ResourceType::JSON;
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
    std::ifstream file(absolutePath, std::ios::in | std::ios::binary);
    std::string str(std::istreambuf_iterator<char>{file}, {});
    Utilities::Array<char> data(str.size());
    memcpy(data.Data(), str.c_str(), str.size());
    return new File(absolutePath, data.Data(), str.size());    
}

} // namespace Engine::Platform::IO

