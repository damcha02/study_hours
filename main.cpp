#include "data_class.hpp"
#include <chrono>
#include <thread>
#include <atomic>


const static std::string path = "data/data.csv";
constexpr int default_delay = 45;

void printTextWithDelay(std::string text, int delay){
    // std::string text = "WELCOME\n";
    std::cout << std::endl;

    for (char c : text) {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay)); // 500ms delay
    }
    std::cout << std::endl;
}

void initializeProgram(){
    printTextWithDelay("FIRST WE NEED TO INITIALIZE THIS PROGRAM.\n", default_delay);
    printTextWithDelay("TO DO THIS PLEASE ENTER YOUR ", default_delay);
}

std::string choseSubject(){

    // std::cout << "Pick a subject to view/edit the workload." << std::endl;
    printTextWithDelay("Pick a subject to view or edit the workload of.", 0);

    std::cout << std::setw(17) << "NUMPDE (1)" << std::setw(17) << "Fluid (2)" //always 7 spaces
              << std::setw(20) << "StatPhys (3)" << std::setw(15) << "WUS (4)" 
              << std::setw(19) << "View All (5)" << std::endl;

    int input = 0;
    std::cout << ">> ";
    std::cin >> input;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << std::string(40, '-') << "\n";

    // TODO: rewrite this more modular

    std::string subject;
    switch(input) {
        case 1:{
            subject = "NUMPDE";
            break;
        }
        case 2: {
            subject = "Fluid";
            break;
        }
        case 3: {
            subject = "StatPhys";
            break;
        }
        case 4: {
            subject = "WUS";
            break;
        }
        case 5: {
            subject = "All";
            break;
        }
        default: {
            std::cout << "no valid subject detected, please run the program again." << std::endl;
            return ":(";
        }
    }
    std::cout << "Selected: " << subject << std::endl;
    return subject;
}

std::string choseAction(std::string subject){
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


//data stored
//NUMPDE: total minutes, minutes of videos, minutes of exercises, amount of old exams
//Fluid: total serien, amount of serien fluid 1, amount of serien fluid 2, amount of old exams
//Stat Phys: total serien+lectures,amount of lectures, amount of serien, amount of old exams
//WUS: total serien, amount of serien, amount of old exams

int main(){
    
    printTextWithDelay("WELCOME TO THE STUDY TRACKER\n", default_delay);

    // printTextWithDelay("WHAT DO YOU WANT TO DO?");
    std::string subject = choseSubject();

    std::string action = choseAction(subject);


    Data data_obj(path);
    data_obj.readMetaData();
    if(action == "View") {
        if(subject == "All"){
            data_obj.viewAllData();
        }
        else data_obj.viewData(subject);
    }
    else if(action == "Edit") data_obj.editData(subject);
    else if(action == "Plan") data_obj.plan(subject);
    else if(action == "quit") return 0;


    return 0;
}

/*TODO: add options to quit the program (q)*/
/*TODO: 
    initialize data with bash script? 


*/