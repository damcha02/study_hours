#pragma once

#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// not from class Data:
std::string convertMinsToHrs(std::string minutes);

class Data {
   private:
    // maybe do
    std::unordered_map<std::string, int>  __key_int_map;
    std::vector<std::string_view>         __int_key_map;
    std::vector<std::vector<std::string>> __data;
    // then: __data.at(__key_int_map("sex")) are the entries
    // your code does not care that the subject is numpde, it only needs to know about entry <N>
    // prolly overkill

   private:
    std::unordered_map<std::string, std::vector<std::string>> all_data_m;
    std::unordered_map<std::string, std::vector<std::string>> label_data_m;
    std::set<std::string>                                     subjects_m;
    std::string                                               filepath_m;

   public:
    Data(std::string_view filepath);
    void        readData();
    std::string chooseSubject();
    std::string chooseAction(std::string subject);
    void        viewData(std::string subject);
    void        viewAllData();
    void        editData(std::string subject);
    void        printCategoriesToEdit(std::string subject);
    void        drawProgressBar(std::string subject);
    void        readMetaData();
    void        plan(std::string subject);
};
