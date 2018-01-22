
#include "LittleSnitchHostCollection.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <sstream>

namespace po = boost::program_options;

// get desktop path
char *GetDesktopPath(void) {
    
    static char real_public_path[PATH_MAX + 1] = {0};
    
    if (real_public_path[0])
        return real_public_path;
    
    strcpy(real_public_path, getenv("HOME"));
    memcpy(real_public_path + strlen(real_public_path), "/Desktop", 8);
    return real_public_path;
    
}

template <typename T>
void remove_duplicates(std::vector<T>& vec)
{
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

// helper class to process command line
bool process_command_line(int argc, char** argv,
                          std::string& outputPath,
                          std::vector<std::string> & targetUrls,
                          bool & copyToClipboard)
{
    
    try {
        
        po::options_description desc("Program Usage", 1024, 512);
        desc.add_options()
        ("help",     "produce help message")
        ("outputPath,o",    po::value<std::string>(&outputPath)->default_value(GetDesktopPath()), "File output path from data extraction")
        ("toClipboard,c",   po::value<bool>()->default_value(true), "Copy result to clipboard")
        ("targetUrls,u",    po::value<std::vector<std::string>>(&targetUrls)->multitoken(), "list of urls which contain host files")
        ;
        
        // Just parse the options without storing them in a map.
        po::parsed_options parsed_options = po::command_line_parser(argc, argv).options(desc).run();
        
        // Build list of multi-valued option instances.
        std::vector<std::vector<std::string>> lists;
        for (const po::option& o : parsed_options.options) {
            if (o.string_key == "targetUrls")
                lists.push_back(o.value);
        }
        
        // Print out the multi-valued option, each separate instance on its own line.
        for (size_t i = 0; i < lists.size(); ++i) {
            for (size_t j = 0; j < lists[i].size(); ++j)
                targetUrls.push_back(lists[i][j]);
        }
        
        po::variables_map vm;
        po::store(parsed_options, vm);
        
        if (vm.count("help")) {
            std::cout << desc << "\n";
            return false;
        }
        
        po::notify(vm);
    }
    catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return false;
    }
    catch(...) {
        std::cerr << "Unknown error!" << "\n";
        return false;
    }
    
    std::stringstream ss;
    return true;
}

// entry point
int main(int argc, char** argv)
{
    
    // configuration variables to pass through to 'LittleSnitchHostCollection' for rule collection
    std::string execPath = boost::filesystem::system_complete(argv[0]).c_str();
    std::string outputPath = GetDesktopPath();
    std::vector<std::string> targetUrls;
    bool copyToClipboard = true;
    
    // default urls which the code currently works with.
    targetUrls.push_back("https://raw.githubusercontent.com/notracking/hosts-blocklists/master/hostnames.txt");
    targetUrls.push_back("https://raw.githubusercontent.com/notracking/hosts-blocklists/master/domains.txt");
    targetUrls.push_back("https://raw.githubusercontent.com/StevenBlack/hosts/master/hosts");
    
    // process commandline arguments
    bool result = process_command_line(argc, argv, outputPath, targetUrls,copyToClipboard);
    
    // remove duplicates so we don't hit the same list twice
    remove_duplicates(targetUrls);
    
    // Now begin collection service
    LittleSnitchHostCollection lshc(outputPath,copyToClipboard,targetUrls);
    
}
