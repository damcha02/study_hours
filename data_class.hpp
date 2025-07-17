#pragma once

#include <iostream>
#include <iomanip>
#include <limits>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string_view>

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>

#include <chrono>
#include <thread>

//not from class Data:
std::string convertMinsToHrs(std::string& minutes);

class Data {
    private:
        std::map<std::string, std::vector<std::string>> all_data_m;
        std::map<std::string, std::vector<std::string>> label_data_m;
        std::set<std::string> subjects_m; 
        std::string data_filepath_m;
        std::string metadata_filepath_m;
    public:
        Data(std::string_view data_filepath, std::string_view metadata_filepath); 
        void readData();
        std::string chooseSubject();
        std::string chooseAction(std::string subject);
        void viewData(std::string subject);
        void viewAllData();
        void editData(std::string subject);
        void printCategoriesToEdit(std::string subject);
        void drawProgressBar(std::string subject);
        void readMetaData();
        void plan(std::string subject);
};


