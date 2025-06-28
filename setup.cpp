#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <limits>

#include <vector>
#include <map>

#include <filesystem>
#include <fstream>

static std::string filepath_metadata = "../data/.metadata.txt";
static std::string filepath_data = "../data/data.csv";

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

void printExample(){
    std::string text = "Here is an example from Danil's 6th semester:\n";
    printTextWithDelay(text, default_delay);

    std::cout << "\nFluid\n" 
            << "\tTotal:          17 material\n" 
            << "\tFluid 1:        6 exercises\n"
            << "\tFluid 2:        6 exercises\n"
            << "\tPast exams:     5 exams\n";
    std::cout << "\nNUMPDE\n"
            << "\tTotal:          7517 mins\n"
            << "\tVideos:         1139 mins\n"
            << "\tExercises:      3707 mins\n";
    std::cout << "\nStatPhys\n"
            << "\tTotal:          26 material\n"
            << "\tVideos:         12 lectures\n"
            << "\tExercises:      9 exercises\n"
            << "\tPast exams:     5 exams\n";
    std::cout << "\nWUS\n"
            << "\tTotal:          17 material\n"
            << "\tExercises:      12 exercises\n"
            << "\tPast exams:     5 exams\n"; 
}

void printInformation(){
    std::string sometext;
    sometext = "The normal setup looks something like this:\n";
    sometext += "Total of material, material 1, material 2, maybe #exams?\n";
    sometext += "The total is all the work of the subject and the other parameters are the to do's\n";
    sometext += "So when you finish one portion of work and update the tracker, the material param will deminish\n";
    sometext += "The total will stay the same and like this the program can calculate your progress and give feedback\n";
    sometext += "\nYou can always check your setup in the directory data and the files metadata.txt and data.csv\n";
    sometext += "\n\t !!! BE CAREFUL TO NOT RUN THE SETUP SCRIPT AGAIN OR ELSE YOUR PREVIOUS DATA MIGHT BE OVERWRITTEN !!!\n";
    printTextWithDelay(sometext, default_delay);

    printExample();
}

std::string takeInput(){
    
    std::string input; 
    std::cout << ">> ";
    
    // std::cin >> input;
    std::getline(std::cin, input);
    if(input.empty()) input = "NAN";
    return input;
}

bool insertOneMore(){
    std::string text = "Would you like to add another parameter? Y/n ";
    printTextWithDelay(text, default_delay);
    
    std::string input = takeInput();
    
    if(input != "y" && input != "Y") return false;
    return true;
}

void insertParameter(std::string subject, dataTYPE& metadata){
    std::string text2 = "What is your next parameter for ";
    text2 += subject;
    text2 += "?\n";
    printTextWithDelay(text2, default_delay);
    
    std::string input = takeInput();
    metadata[subject].push_back(input);
    
    std::string unit_text = "What unit is your ";
    unit_text += input;
    unit_text += "?\n";
    printTextWithDelay(unit_text, default_delay);
    
    std::string unit = takeInput();
    metadata[subject].push_back(unit);
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
    if(ready != "Y" && ready != "y"){ throw std::runtime_error("Read the instructions again if not ready or ask customer support");}
    
    bool done = false;
    
    while(!done){
        printTextWithDelay("Do you want to add a new subject? Y/n ", default_delay);
        std::string add_new = takeInput();
        //TODO add finish text
        if(add_new != "Y" && add_new != "y"){ std::cout << "Metadata setup is finished" << std::endl; done = true; continue;}
    
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
    
        while(insertOneMore()) {insertParameter(subject_one, metadata);}
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

    //write rest of data
    std::ofstream mydata(filepath_data);
    if(!mydata.is_open()) throw std::runtime_error("data.csv file not found");
    
    //iterate over metadata and ask for each part the exact number or do default 0
    for(auto it : metadata){
        std::cout << "\n--------------------------------------------------------------------\n";
        std::cout << "\nLet's insert the trackable data for -> " << it.first << std::endl;
        std::cout << "If you don't know the exact parameter you can just insert 0 and change it later in the data.csv file\n" << std::endl;
        subjects[it.first] = {};
        for(int i = 0; i < it.second.size() - 1; i += 2){
            std::cout << "Insert amount of " << it.second[i + 1] << " for the " << it.second[i] << " parameter\n";
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
    std::cout << "The setup is fully finished, you can now run the normal tracker and view your progress\n\n" 
        << "ENJOY :)" << std::endl;
    return 0;
}