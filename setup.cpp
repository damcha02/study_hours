#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <limits>

#include <vector>
#include <map>

#include <filesystem>
#include <fstream>
#include "data_class.hpp"

static std::string_view filepath_instructions = "../assets/_setup_instructions.txt";
static std::string filepath_metadata = "../data/.metadata.txt";
static std::string filepath_data = "../data/data.csv";

//example files
const constexpr std::string_view example_path = "../assets/_example.csv";
const constexpr std::string_view exampleMetapath = "../assets/_example.txt";

static int default_delay = 10;
typedef std::map<std::string,std::vector<std::string>> dataTYPE;

namespace fs = std::filesystem;
void flushGarbageInput() {
    std::cin.clear();
    while (std::cin.rdbuf()->in_avail() > 0) {
        char ch = std::cin.get();
        if (ch == '\n') break;
    }
}

void printTextWithDelay(std::string text, int delay){
    // std::string text = "WELCOME\n";
    std::cout << std::endl;
    
    for (char c : text) {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay)); // 500ms delay
    }
}

void printInformation(){
    std::string filepath(filepath_instructions);
    std::fstream instructionFile;
    instructionFile.open(filepath, std::ios::in);

    if(!instructionFile.is_open()){
        std::cout << "Failed to open file: " << filepath << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::string line;

    while(std::getline(instructionFile, line)){
        printTextWithDelay(line, default_delay);
    }

    std::string text = "Here is an example from Danil's 6th semester:\n";
    printTextWithDelay(text, default_delay);
    Data example_obj(example_path, exampleMetapath);
    example_obj.viewAllData();
}

std::string takeInput(){
    
    std::string input; 
    std::cout << ">> ";
    
    // std::cin >> input;
    std::getline(std::cin, input);
    if(input.empty()) input = "NAN";
    return input;
}

void insertParameter(std::string subject, dataTYPE& metadata){
    bool insertOneMore = true;
    while(insertOneMore){
        std::string text = "Would you like to add another parameter? Y/n ";
        printTextWithDelay(text, default_delay);
        
        std::string input = takeInput();

        if(input == "N" || input == "n") {insertOneMore = false; continue;}
        if((input != "N" && input != "n") && (input != "Y" && input != "y")) 
        {try {throw std::runtime_error("incorrect input, try again");}
                                                catch(const std::exception& e){
                                                    std::cerr << e.what() << std::endl; continue;
                                                }
        }

        std::string text2 = "What is your next parameter for ";
        text2 += subject;
        text2 += "?\n";
        printTextWithDelay(text2, default_delay);
        
        std::string parameter = takeInput();
        metadata[subject].push_back(parameter);
        
        std::string unit_text = "What unit is your ";
        unit_text += parameter;
        unit_text += "?\n";
        printTextWithDelay(unit_text, default_delay);
        
        std::string unit = takeInput();
        metadata[subject].push_back(unit);

    }

}

bool checkIfNumberInput(std::string input){
    for(char c : input){
        if(c < 48 || c > 57){
            std::cout << "The parameter has to be a number, please enter it again." << std::endl;
            return false;
        }
    }
    return true;
}

void write_metadata(dataTYPE& metadata){
    
    std::ofstream mymetadata(filepath_metadata);
    if(!mymetadata.is_open()) throw std::runtime_error(".metadata.txt file not found");
    printTextWithDelay("(A Data file has been created, you can modify it later, but please be careful.)\n", default_delay);
    
    printTextWithDelay("You can add as many subjects as you want", default_delay);
    
    printInformation();
    
    flushGarbageInput();
    
    printTextWithDelay("Are you ready to set up your own tracker? Y/n ", default_delay);
    std::string ready = takeInput();
    if(ready != "Y" && ready != "y"){ try {throw std::runtime_error("Read the instructions again if not ready or ask customer support");} 
        catch(const std::exception& e){
            std::cerr << e.what() << std::endl;
        }
        std::exit(EXIT_FAILURE);
    }
    
    bool done = false;
    
    while(!done){
        printTextWithDelay("Do you want to add a new subject? Y/n ", default_delay);
        std::string add_new = takeInput();
        //TODO add finish text
        if(add_new == "N" || add_new == "n"){ std::cout << "Metadata setup is finished" << std::endl; done = true; continue;}
        if((add_new != "N" && add_new != "n") && (add_new != "Y" && add_new != "y")) 
        {try {throw std::runtime_error("incorrect input, try again");}
                                                catch(const std::exception& e){
                                                    std::cerr << e.what() << std::endl; continue;
                                                }
        }
    
        printTextWithDelay("You can now input the name of the subject to add\n", default_delay);
        std::string subject_one = takeInput();
        metadata[subject_one] = {};
        
        std::string text = "The program is defaulted to start with Total, what unit does your total for "; 
        text += subject_one;
        text += " have? (i.e mins? exercises? material? etc...)\n";
        printTextWithDelay(text, default_delay);
        metadata[subject_one].push_back("Total");
    
        std::string unit_total = takeInput();
        metadata[subject_one].push_back(unit_total);

        insertParameter(subject_one, metadata);
    }
    
    //write metadata to metadatafile
    for(auto subject : metadata){
        mymetadata << subject.first << ",";
        for(auto entry : subject.second){
            mymetadata << entry << ",";
        }
        mymetadata << "\n";
    }
    mymetadata.close();
}

void rename_parent_dir(int argc, char **argv){
    namespace fs = std::filesystem;
    
    fs::path cwd = fs::current_path();
    fs::path old_dir = cwd.parent_path();
    fs::path parent_of_old_dir = old_dir.parent_path();

    fs::path new_dir = parent_of_old_dir / argv[1];

    if(old_dir.filename().string() == new_dir.filename().string()){
       std::cerr << "Directory with this name already exists\n";
       std::cout << "This might overwrite all your previously configured data files, do you still want to continue? Y/n ";
       std::string input;
       std::cin >> input;
       if(input != "Y" && input != "y"){
            std::exit(EXIT_FAILURE);
       }
    }
    
    try{
        fs::rename(old_dir, new_dir);
        std::cout << "Renamed “"
                << old_dir.filename().string()
                << "” to “" << argv[1] << "”\n";
    } catch (const fs::filesystem_error& e){
        throw std::ios_base::failure(
            std::string("Failed to rename ‘") +
            old_dir.string() + "’ to ‘" + new_dir.string() + "’: " + e.what());    
    }
}

void write_data(dataTYPE& metadata, dataTYPE& subjects){
//write rest of data
    std::ofstream mydata(filepath_data);
    if(!mydata.is_open()) throw std::runtime_error("data.csv file not found");
    
    //iterate over metadata and ask for each part the exact number or do default 0
    for(auto it : metadata){
        std::cout << "\n--------------------------------------------------------------------\n";
        std::cout << "\nLet's insert the trackable data for -> " << it.first << std::endl;
        std::cout << "If you don't know the exact parameter you can just insert 0 and change it later in the data.csv file\n" << std::endl;
        subjects[it.first] = {}; //intialize an empty vector
        
        //Insert the Total
        std::cout << "Insert amount of " << it.second[1] << " which is the Total work that there is for this subject\n";
        std::string input = takeInput();
        //how to check if its a number or not?
        if(!checkIfNumberInput(input)){
            do{
                input = takeInput();

            }while(!checkIfNumberInput(input));
        }
        //input should be good here
        subjects[it.first].push_back(input);    

        for(int i = 2; i < it.second.size() - 1; i += 2){
            std::cout << "Insert amount of " << it.second[i + 1] << " that you still have left to do for the " << it.second[i] << " parameter\n";
            std::string input = takeInput();
            //how to check if its a number or not?
            if(!checkIfNumberInput(input)){
                do{
                    input = takeInput();

                }while(!checkIfNumberInput(input));
            }
            //input should be good here
            subjects[it.first].push_back(input);
        }
    }

    for(auto it : subjects){
        mydata << it.first << ",";
        for(auto num : it.second){
            mydata << num << ",";
        }
        mydata << "\n";
    }

    
    mydata.close();
}

int main(int argc, char **argv){  
    if (argc > 3) {
        std::cerr << "Usage: " << argv[0] << " new_name\n";
        return 1;
    }
    if(argc > 1) {
        rename_parent_dir(argc, argv);
    }
    
    std::map<std::string, std::vector<std::string>> metadata;
    std::map<std::string, std::vector<std::string>> subjects;

    printTextWithDelay("WELCOME TO THE PROGRESS TRACKER YOU DIDN'T NEED, BUT NOW YOU HAVE IT. :)\n", default_delay);
    printTextWithDelay("Let's start by setting up your data to track.", default_delay);    
    //create a directory 
    fs::create_directory("../data");

    write_metadata(metadata);

    write_data(metadata, subjects);
    
    std::cout << "The setup is fully finished, you can now run the normal tracker and view your progress\n\n" 
        << "ENJOY :)" << std::endl;
    return 0;
}