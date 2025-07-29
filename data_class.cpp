#include "data_class.hpp"


std::string convertMinsToHrs(std::string& minutes){
    double mins = std::stod(minutes);
    double hrs = mins / 60.0;
    std::string hours = std::to_string(hrs);
    return hours;
}

Data::Data(std::string_view data_filepath, std::string_view metadata_filepath) : data_filepath_m(data_filepath),
                                                                                 metadata_filepath_m(metadata_filepath){
    readData(); 
    readMetaData();
} //ctor

void Data::readData(){
    std::fstream fin;
    fin.open(data_filepath_m, std::ios::in);
    if (!fin.is_open()) {
        std::cout << "Failed to open file: " << data_filepath_m << std::endl;
        return;
    }
    std::vector<std::string> data;
    std::string line, word;

    while(std::getline(fin, line)){
        data.clear();

        std::stringstream s(line);

        while(getline(s, word, ',')){
            
            data.push_back(word);
        }
        all_data_m[data[0]] = std::vector<std::string>(data.begin() + 1, data.end());
    }
}
std::string Data::chooseSubject(){
    std::cout << "Pick a subject to view or edit the workload of." << std::endl;
    std::vector<std::string> vec;
    int count = 1;
    for(auto it : all_data_m){
        std::cout << "\t" << it.first << std::setw(20 - it.first.size()) << "(" << count << ")\n";
        vec.push_back(it.first);
        ++count;
    }
    std::cout << "\t" << "View All" << std::setw(20 - 8) << "(" << count << ")\n" << std::endl;
    vec.push_back("All");

    int input;
    std::cout << ">> ";
    std::cin >> input;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if(input > vec.size() || input < 0){
        throw std::runtime_error("invalid input, try again.");
    }
    std::cout << "Selected: " << vec[input - 1] << std::endl;
    return vec[input - 1];
}

std::string Data::chooseAction(std::string subject){
    bool flag = (subject == "All") ? false : true;
    std::string action;

    if(flag == true) {
        std::cout << "\nPick your action." << std::endl;
        std::cout << std::setw(15) << "View (1)" << std::setw(15) << "Edit (2)" << std::setw(15) << "Plan (3)" << std::setw(15) << "Quit (q)" << std::endl; 
        std::string input; 
        std::cout << ">> ";
        std::getline(std::cin, input);
        std::cout << std::string(40, '-') << "\n";

        if(input == "q" || input == "Q"){
            return "quit";
        }
        if(input == "2"){
            action = "Edit";
        } else if(input == "1" || input.empty()){
            action = "View";
        } else if(input == "3"){
            action = "Plan";
        }
        else {
            std::cout << "no valid action detected, please run the program again." << std::endl;
            action = ":(";
        }
    } else action = "View";

    std::cout << "Selected: " << action << '\n' << std::endl;
    return action;
}


void Data::readMetaData(){
    //reads from .metadata.txt
    // std::string metadata = "../data/.metadata.txt";
    std::fstream fin;
    fin.open(metadata_filepath_m, std::ios::in);
    if (!fin.is_open()) {
        std::cout << "Failed to open file: " << metadata_filepath_m << std::endl;
        return;
    }
    std::vector<std::string> data;
    std::string line, word;

    while(std::getline(fin, line)){
        data.clear();

        std::stringstream s(line);

        while(getline(s, word, ',')){
            data.push_back(word);
        }
        subjects_m.insert(data[0]);
        label_data_m[data[0]] = std::vector<std::string>(data.begin() + 1, data.end());
    }
    if(label_data_m.size() < 1) {
        std::cout << "there is a lack of metadata" << std::endl; 
        std::exit(EXIT_FAILURE);
    }
}
void Data::plan(std::string subject){
    viewData(subject);

    int total_left = 0;
    for(int i = 1; i < all_data_m[subject].size(); ++i){
        total_left += std::stoi(all_data_m[subject][i]);
    }
    std::string totalstr = std::to_string(total_left);
    std::cout << "\nTotal left: " << total_left << " " << label_data_m[subject][1];
    if(label_data_m[subject][1] == "mins") {
        std::cout << " or " << convertMinsToHrs(totalstr) << " hrs.";
    }
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "How many days do you have left for " << subject << "? >> ";
    double input;
    std::cin >> input;

    //fetch total amount of work left

    double work_per_day = static_cast<double>(total_left) / input;

    std::cout << "\nTo finish all work in " << input << " days, you need to do " 
    << work_per_day << " " << label_data_m[subject][1] << "/day";
    std::string workstr = std::to_string(work_per_day);
    if(label_data_m[subject][1] == "mins") {
        std::cout << " or " << convertMinsToHrs(workstr) << " hrs/day";
    }
    std::cout << std::endl;
} 
std::vector<int> findOutAlignment(std::vector<std::string> metadata, int buffer = 0)
{
    std::vector<int> aligment;
    // int longest_string_size = 0;
    // int longest_string_idx = 0;

    aligment.push_back(metadata[0].size() + 5);
    aligment.push_back(26 - metadata[0].size());
    for(int i = 2; i < metadata.size() - 1; i += 2){
        int size = metadata.at(i).size();
        if(i % 2 == 0) {
            aligment.push_back(size + 5);
            aligment.push_back(26 - size);
        }

        // if(size > longest_string_size) {longest_string_size = size; longest_string_idx = i;}
    }
    return aligment;
}

void Data::viewData(std::string subject){
    
    std::vector<int> alignment = findOutAlignment(label_data_m[subject]);
    std::cout << subject; drawProgressBar(subject);
    
    //print the total amount that is left
    int total_left = 0;
    for(int i = 1; i < all_data_m[subject].size(); ++i){
        total_left += std::stoi(all_data_m[subject][i]);
    }
    std::cout << std::setw(alignment[0] + 5) << "Total Left" << std::setw(alignment[1] - 5) << "" << total_left << " " << label_data_m[subject][1];
    if(label_data_m[subject][1] == "mins"){
        std::string mins = std::to_string(total_left);
        std::cout << std::setw(5) << "" << convertMinsToHrs(mins) << " hrs";
    }
    std::cout << std::endl;

    for(int i = 1, j = 2; i < all_data_m[subject].size(); ++i, j += 2){
        std::cout << std::setw(alignment[j]) << label_data_m[subject][j] << std::setw(alignment[j + 1]) << "" << all_data_m[subject][i] << " " << label_data_m[subject][j + 1];
        if(label_data_m[subject][j + 1] == "mins"){
            std::cout << std::setw(5) << "" << convertMinsToHrs(all_data_m[subject][i]) << " hrs";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Data::viewAllData(){
    for(auto i : subjects_m){
        viewData(i);
        std::cout <<std::endl;
    }
}

void Data::printCategoriesToEdit(std::string subject){
    std::vector<int> alignement = findOutAlignment(label_data_m[subject], 5);
    for(int i = 0, j = 0; i < all_data_m[subject].size(); ++i, j += 2){
        std::cout << std::setw(2) << "(" << i << ") " << label_data_m[subject][j] << std::setw(alignement[j + 1]) << "" << all_data_m[subject][i] << " " << label_data_m[subject][j + 1] << std::endl;
    }
}

void Data::editData(std::string subject){

    std::cout << "\nPick a category to edit." << std::endl;
    printCategoriesToEdit(subject);
    int category = 0;
    std::cout << ">> ";
    std::cin >> category;
    std::cout << std::string(40, '-') << "\n";
    if(category >= all_data_m[subject].size()) {
        std::cout << "invalid category, please run the program again." << std::endl;
        return;
    }
    std::cout << "\nHave you finished work (0) or do you want to add work (1)?" << std::endl;
    int work = 0;
    std::cout << ">> ";
    std::cin >> work;
    std::cout << std::string(40, '-') << "\n";

    int amount = 0;
    int value = 0;
    std::string new_val;
    if(work == 0){
        std::cout << "\nPlease enter the amount of work you finished" << std::endl;
        std::cout << ">> ";
        std::cin >> amount;
        std::cout << std::string(40, '-') << "\n";
        value = std::stoi(all_data_m[subject][category]) - amount;

        new_val = std::to_string(value);
        all_data_m[subject][category] = new_val;
    }
    else if(work == 1){
        std::cout << "\nPlease enter the amount of work you want to add" << std::endl;
        std::cout << ">> ";
        std::cin >> amount;
        std::cout << std::string(40, '-') << "\n";
        value = std::stoi(all_data_m[subject][category]) + amount;

        new_val = std::to_string(value);
        all_data_m[subject][category] = new_val;
    }
    //throw error TODO
    else {std::cout << "invalid option, please run the program again." << std::endl; return;}

    //write to file
    std::ofstream fout(data_filepath_m);
    for(auto i : all_data_m){
        fout << i.first << ",";
        for(auto j : i.second){
            fout << j << ",";
        }
        fout << "\n";
    }
    fout.close();

    std::cout << "CSV updated" << std::endl;
    viewData(subject);

}

//only the first two after the total are relevant, 
void Data::drawProgressBar(std::string subject){

    int size = subject.size();
    std::cout << std::setw(20 - size) << "";

    double total = std::stod(all_data_m[subject][0]);
    double done = total;
    for(int i = 1; i < all_data_m[subject].size(); ++i){
        done -= std::stod(all_data_m[subject][i]);
    }

    double percent = (done / total) * 100;
    int width = 20;  // total width of the bar
    int pos = percent / 100 * width;

    std::cout << "[";
    for (int i = 0; i < width; ++i) {
        if (i < pos) std::cout << "=";
        else std::cout << " ";
    }
    std::cout << "] " << int(percent) << " %\n";

    //TODO: add total - total left / totall
    std::cout << "\t\t\t" << done << "/" << total << " " << label_data_m[subject][1] << std::endl;


    std::cout << std::endl;
}