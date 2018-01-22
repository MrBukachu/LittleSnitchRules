#ifndef LittleSnitchHostCollection_hpp
#define LittleSnitchHostCollection_hpp

#include <stdio.h>
#include <string>

enum parseType {host, domain};
typedef std::vector<std::string> domainStore;

class LittleSnitchHostCollection {
public:
    
    LittleSnitchHostCollection(std::string outputPath, bool CopyToClipboard, std::vector<std::string> urls);
    ~LittleSnitchHostCollection();
    
    std::string OutputPath();
    std::string LittleSnitchRules();
    
    void SetOutputPath(std::string path);
    
private:
    
    std::string buildSnitchRules(std::string & line, parseType pt);
    void eraseSubStr(std::string & mainStr, const std::string & toErase);
    
};

#endif /* LittleSnitchHostCollection_hpp */
