#include "LittleSnitchHostCollection.hpp"
#include <iostream>
#include "HTTPDownloader.hpp"
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>

const std::string blankIp = "0.0.0.0 ";
std::vector<std::string> DomainList;
std::string rules;

template <typename T>
void remove_duplicates(std::vector<T>& vec)
{
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

// extracts data out of string
void ParseOutDomainsAndHosts(std::string line){
    
    // logic to determine if we're dealing with host or domains
    bool isHostFile = (line.rfind(blankIp, 0) == 0);
    bool isAddress = (line.rfind("address", 0) == 0);
    
    // transform 'line' string into
    if (isHostFile) {
        size_t pos = line.find(blankIp);
        if (pos != std::string::npos)
            line.erase(pos, blankIp.length());
    } else if (isAddress) {
        size_t s = line.find("/");
        size_t e = line.find("/", s + 1);
        std::string sub = line.substr(s + 1, e - s -1);
        line = sub;
    } else { return; }
    
    // push line into vector
    if (line.size() > 0)
        DomainList.push_back(line);
}

std::string LittleSnitchRules(){
    
    std::string littleSnitchRules;
    for(auto domain : DomainList){
        littleSnitchRules += "action: deny \ndirection: outgoing \npriority: regular \nprocess: any \nowner: me \ndestination: " + domain + "\nport: any \nprotocol: any \n\n";
    }
    
    return littleSnitchRules;
}

// grab data and store into vectors
void fetch_data(std::string outputPath, bool CopyToClipboard, std::vector<std::string> urls){
    
    // Initiate class that helps us download file from url
    HTTPDownloader downloader;
    
    // download and store bad domains
    std::string fileContents;
    for (std::string source : urls) {
        
        std::string fileContents = downloader.download(source);
        typedef boost::tokenizer<boost::char_separator<char> > line_tokenizer;
        line_tokenizer tok(fileContents, boost::char_separator<char>("\n\r"));
        
        for (line_tokenizer::const_iterator i = tok.begin(), end = tok.end(); i != end ; ++i){
            std::string rule = boost::lexical_cast<std::string> (*i);
            if (rule.size() > 0 && rule.at(0) != '#') {
                ParseOutDomainsAndHosts(rule);
            }
        }
    }
    
    // remove duplicates from vector
    remove_duplicates(DomainList);
    
    // build rules
    rules = LittleSnitchRules();
    
    // output to file
    std::ofstream file(outputPath + "littleSnitchOut.txt");
    file << rules;
    
}

LittleSnitchHostCollection::LittleSnitchHostCollection(std::string outputPath, bool CopyToClipboard, std::vector<std::string> urls) {
    fetch_data(outputPath, CopyToClipboard, urls);
}

LittleSnitchHostCollection::~LittleSnitchHostCollection() {
    
}


