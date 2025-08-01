#include "data_class.hpp"
#include <chrono>
#include <thread>
#include <atomic>
#include <cstdlib>


const constexpr std::string_view data_path                = "../data/data.csv";
const constexpr std::string_view metadata_path       = "../data/.metadata.txt";
static constexpr int             default_delay       = 45;

void printTextWithDelay(std::string_view text, int delay){
    // std::string_view text = "WELCOME\n";
    std::cout << std::endl;

    for (char c : text) {
        std::cout << c << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay)); // 500ms delay
    }
    std::cout << std::endl;
}

// void initializeProgram(){
//     printTextWithDelay("FIRST WE NEED TO INITIALIZE THIS PROGRAM.\n", default_delay);
//     printTextWithDelay("TO DO THIS PLEASE ENTER YOUR ", default_delay);
// }


void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
//data stored
//NUMPDE: total minutes, minutes of videos, minutes of exercises, amount of old exams
//Fluid: total serien, amount of serien fluid 1, amount of serien fluid 2, amount of old exams
//Stat Phys: total serien+lectures,amount of lectures, amount of serien, amount of old exams
//WUS: total serien, amount of serien, amount of old exams

int main(){
    clearScreen();
    
    printTextWithDelay("WELCOME TO THE STUDY TRACKER\n", default_delay);

    // printTextWithDelay("WHAT DO YOU WANT TO DO?");
    
    Data data_obj(data_path, metadata_path);
    bool done = false;
    while(!done){
        std::string subject = data_obj.chooseSubject();
        std::string action = data_obj.chooseAction(subject);


        if(action == "View") {
            if(subject == "All"){
                data_obj.viewAllData();
            }
            else data_obj.viewData(subject);
        }
        else if(action == "Edit") 
            data_obj.editData(subject);
        else if(action == "Plan") 
            data_obj.plan(subject);
        else if(action == "quit") 
            done = true;
    
        printTextWithDelay("\n\n(c) continue \t\t (q) quit", default_delay);
        std::cout << ">> ";
        std::string input;
        std::cin >> input;
        done = (input == "c" || input == "C") ? false : true;
        if(!done) clearScreen();
    }

    std::cout << "Goodbye" << std::endl;
    return 0;
}
