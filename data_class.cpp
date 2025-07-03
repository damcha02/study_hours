#include "data_class.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using std::max;

// defined twice
static std::string convertMinsToHrs(const std::string& minutes) {
    double      mins  = std::stod(minutes);  // double mins; from_chars(minutes.data(), minutes.size(), mins); is technically faster
    double      hrs   = mins / 60.0;
    std::string hours = std::to_string(hrs);
    return hours;
}

/* https://en.cppreference.com/w/cpp/filesystem/path.html :D */
Data::Data(std::string_view filepath) : filepath_m(filepath) {
    filepath_m = "../" + filepath_m;

    readData();
    readMetaData();
}  // ctor

void Data::readData() {
    std::fstream fin{filepath_m, std::ios::in};
    if (!fin.is_open()) {
        std::cout << "Failed to open file: " << filepath_m << '\n';
        return;
    }

    /*
    while line {
        ss = sstream(line);
        std::string word;
        getline(ss, word);
        auto& vec = all_data[key]; // inserts key and returns the vec
        while (getline(ss, word)) {
            vec.emplace_back(word);
            // emplace_back:  compiler will never write to word, will directly use the vector memory
            // push_back: compiler writes data do word, creates space in vec for new word, copy word to vec
            //  also: creates space means: new size = 2 x old_size (!)
            //  so if your vec is 1GB and you trigger a realloc it will be 2GB even though you only added ":(" to the vec lol
        }
    }
     */

    std::vector<std::string> tokens;
    std::string              line;
    while (std::getline(fin, line)) {
        tokens.clear();

        std::stringstream s{line};
        std::string       word;
        while (std::getline(s, word, ',')) {
            tokens.emplace_back(word);  // push_back = 3 allocations, emplace back = 1 alloc
        }
        // crashes (i think) if a line does not contain ',', also if tokens is empty then begin > end
        all_data_m[tokens[0]] = std::vector<std::string>(tokens.begin() + 1, tokens.end());
    }
}

std::string Data::chooseSubject() {
    std::cout << "Pick a subject to view or edit the workload of." << '\n';
    std::vector<std::string> vec;

    const auto printer = [](std::string_view key, const auto& v) {
        const int width = max(0UL, 20 - key.size());
        std::cout << "\t" << key << std::setw(width) << "(" << v.size() << ")\n";
        // cpp23 ad:
        // const auto index = vec.size();
        // std::print("\t{}{:>{}}({})\n", key, "", width, index);
    };

    for (const auto& [key, _] : all_data_m) {
        printer(key, vec);
        vec.emplace_back(key);
    }
    printer("View All", vec);
    vec.emplace_back("All");

    int input{-1};
    std::cout << ">> ";
    std::cout.flush();  // fucky shit can happen without this

    while (input == -1) {  // no throw, no crash, forces user to behave
        std::cin >> input;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (input > vec.size() || input < 0) {
            std::cout << "invalid input, try again.\n";
            input = -1;
        }
    }

    std::cout << "Selected: " << vec[input - 1] << '\n';
    return vec[input - 1];
}

std::string Data::chooseAction(std::string subject) {
    using std::cout;
    const auto print_line  = [](const auto&... vals) { (std::cout << ... << vals) << '\n'; };
    const auto print_table = [](const auto&... vals) {
        constexpr int width = 15;
        ((std::cout << std::setw(width) << vals), ...);
        std::cout << '\n';
    };

    // sry, got carried away in this function...
    // basically:
    // your program will have some fixed set of options/actions
    // if you extract them to something like:
    /*
    struct Action {
        std::string_view name;
        std::string_view cmd;
        // ...

        // if you want to make it work with a map s.t. the key is 'name', you need operator< and operator== or smth like that. maybe std::hash<Action>
    specialisation? i forgor
    };

    you can have one global settings map/set

    // then some global settings thingy:
    static const std::unordered_map<Action, HandlerFuncion> my_actions = {
        {
            {"View", "1"},
            [](auto whatever) {}
        },
        {
            {"Edit", "2"},
            editFunctionDefinedElsewhere
        }
    };
    // (if you dont add handler functions this can just be a set<Action>), then you need (operator< or operator>) and operator==

    choseAction can then just be something like:

    auto choseAction(std::string subject) {
        // boilderplate stuff
        // get user input

        auto action_it = my_actions.find(user_input);
        if (action_it == my_actions.end()) {
            return "idiot, the option does not exist!\n";
        }

        if you add handler functions {
            const auto& [action, handler] = action_it;
            handler(subject);
            return action.name;
        } else {
           return action->name;
        }
    }

    if you add new features (plot graph etc, idk) you can just add:
        {{"Plot", "p"}, plotterFunc} to my_actions and you are done:)
    */

    // clang-format off
    static const std::map<std::string, std::string> options = { // constexpr in cpp23
        {"1", "View"},
        {"2", "Edit"},
        {"3", "Plan"},
        {"q", "Quit"},
    };
    // clang-format on

    auto action = [&]() -> std::string_view {
        if (subject == "All") {
            return "View";
        }

        print_line("\nPick your action.");
        for (const auto& [key, val] : options) {
            print_table(val + " (" + key + ")");
        }

        std::string input;
        cout << ">> ";
        std::getline(std::cin, input);
        print_line(std::string(40, '-'));

        auto opt = options.find(input);
        if (opt == options.end()) {
            cout << "no valid action detected, please run the program again." << '\n';
            return ":(";
        }

        return opt->second;
    }();

    print_line("Selected: ", action);
    return std::string{action};
}

// this function is the "same" as readData, just do one: readData(path) or whatever and then use it in readMetaData and in readData
// like its not equivalent but its 97.6% identical
void Data::readMetaData() {
    // reads from .metadata.txt
    std::string  metadata = "../data/.metadata.txt";
    std::fstream fin;
    fin.open(metadata, std::ios::in);
    if (!fin.is_open()) {
        std::cout << "Failed to open file: " << metadata << '\n';
        return;
    }

    std::vector<std::string> data;
    std::string              line;

    while (std::getline(fin, line)) {
        data.clear();  // just do vector<str> data{}; in here, compiler will not reallocate on each loop and keeps it cleaner

        std::stringstream s(line);

        std::string word;
        while (getline(s, word, ',')) {
            data.emplace_back(word);
        }
        subjects_m.insert(data[0]);                                                      // explodes if data.empty()
        label_data_m[data[0]] = std::vector<std::string>(data.begin() + 1, data.end());  // explodes if data.empty()

        // map::operator[] inserts the object if it does not exist (retarded decision by cpp...)
        //  see (1): https://en.cppreference.com/w/cpp/container/map/operator_at

        // so for a map m = {{a, 1}, {b, 2}}
        // m[c] = 3 will insert the key c into the map
        // m.at(c) will throw if the key does not exist
    }
}

void Data::plan(std::string subject) {
    viewData(subject);

    {  // instead of writing all_data_m[subject] all the time do
        if (all_data_m.find(subject) == all_data_m.end()) {
            throw "error or handle however";
        }

        auto& data = all_data_m.at(subject);

        // cpp23 advertisement:
        // if (!all_data_m.contains(subject)) { ... }
    }

    // maybe store the data as what it is instead of strings, std::stoi and std::to_string is expensive and technically not necessary
    int total_left = 0;
    for (int i = 1; i < all_data_m[subject].size(); ++i) {
        total_left += std::stoi(all_data_m[subject][i]);  // throws if data is not number: std::isdigt(char)
    }
    std::string totalstr = std::to_string(total_left);
    std::cout << "\nTotal left: " << total_left << " " << label_data_m[subject][1];  // if size == 1 then boom
    if (label_data_m[subject][1] == "mins") {
        std::cout << " or " << convertMinsToHrs(totalstr) << " hrs.";
    }
    std::cout << std::endl;  // std::endl = { std::flush; std::cout << '\n'; }, flush is slow, use '\n'
    std::cout << std::endl;

    std::cout << "How many days do you have left for " << subject << "? >> ";
    double input;
    std::cin >> input;
    /*
    wrap the "? >> " or " >> " somewhere.
    smth like:
    namespace io { // or class/struct, whatever
        constexpr void prompt_user(const auto&... args, auto os = std::cout);
        constexpr print_table();
        constexpr print_line();

        or even cooler:

        struct IoPrintable {
            virtual std::ostream& print(std::ostream& os) const = 0;
            virtual ~IoPrintable() = default;
        };

        inline std::ostream& operator<<(std::ostream& os, const IoPrintable& p) {
            return p.print(os);
        }

        template <int Width, char Chr>
        struct Separator : IoPrintable {
            std::ostream& print(std::ostream& os) const override {
                for (int i = 0; i < Width; ++i) {
                    os.put(Chr);
                }
                if constexpr (Chr != '\n') {
                    os.put('\n');
                }
                return os;
            }
        };

        // to use like: cout << io::thin40
        inline constexpr Separator<1, '\n'> nl{};
        inline constexpr Separator<40, '-'>       thin40{};
        inline constexpr Separator<40, '='>      thick40{};

        template <int Width = -1, typename... Args>
        struct TableRow : IoPrintable {
            std::tuple<Args...> args;

            std::ostream& print(std::ostream& os) const override {
                std::apply([&](const auto&... vals) {
                    ((os << (Width > 0 ? std::setw(Width) : std::setw(0)) << vals), ...);
                    os << '\n';
                }, args);
                return os;
            }
        };

        template <int Width = -1, typename... Args>
        auto table(Args&&... args) {
            return TableRow<Width, std::decay_t<Args>...>{
                std::make_tuple(std::forward<Args>(args)...)
            };
        }

    }

    then:
    std::cout << "test" << io::nl
        << "thin line: " << io::thin40
        << "thick line" << io::thick40
        << table<15>("wow", "a", "table", 69, 4.20) << io::nl;

    :> test
    :> thin line: ----------------------------------------
    :> thick line: ========================================
    :> wow             a              table          69             4.20
    */

    // fetch total amount of work left
    double work_per_day = static_cast<double>(total_left) / input;  // func: getTotalWork(data or smth)

    std::cout << "\nTo finish all work in " << input << " days, you need to do " << work_per_day << " " << label_data_m[subject][1] << "/day";
    std::string workstr = std::to_string(work_per_day);
    if (label_data_m[subject][1] == "mins") {  // explodes[1]
        // duplicate func: convertMinsToHrs
        std::cout << " or " << convertMinsToHrs(workstr) << " hrs/day";
    }
    std::cout << '\n';
}
// ADD SPACE BETWEEN FUNCTIONS T.T
std::vector<int> findOutAlignment(std::vector<std::string> metadata,
                                  int buffer = 0) {  // const vec&, else copy, also put default arg {buffer = 0} in header not cpp + its never used
    std::vector<int> aligment;                       // {}
    // int longest_string_size = 0;
    // int longest_string_idx = 0;
    aligment.push_back(metadata[0].size() + 5);  // explodes if metadata is empty, also use .front(), also why not put it in the loop below?
    aligment.push_back(26 - metadata[0].size());
    for (int i = 2; i < metadata.size() - 1; i += 2) {
        int size = metadata.at(i).size();  // nice use of at
        if (i % 2 == 0) {                  // always == 0, you start at 2 and add 2 each step
                                           // proof by induction (gpt):
                                           // base case: i = 2 → 2 % 2 == 0 ✔
                                           //
                                           // inductive step:
                                           // assume i = 2 * n → i % 2 == 0
                                           // then next i = i + 2 = 2 * (n + 1) → still divisible by 2 → (i + 2) % 2 == 0 ✔
                                           //
                                           // qed, modulo check is redundant
            aligment.push_back(size + 5);
            aligment.push_back(26 - size);
        }

        // if(size > longest_string_size) {longest_string_size = size; longest_string_idx = i;}
    }
    /*
    // obligatory cpp23 advertisement, does the same, looks cooler lol
    auto findOutAlignment(const std::vector<std::string>& metadata, int buffer = 0)
        std::vector<int> alignment;
        alignment.reserve(metadata.size());
        auto cleaned = metadata
                 | std::ranges::views::stride(2)
                 | std::views::transform([](const std::string& s) {
                       return s.size();
                   });

        for (int size : cleaned) {
            aligment.push_back(size + 5);
            aligment.push_back(26 - size);
        }

        aligment.shrink_to_fit();
        return aligment;
    }
    */

    return aligment;
}

void Data::viewData(std::string subject) {
    std::vector<int> alignment = findOutAlignment(label_data_m[subject]);  // operator[] bad for maps
    // again do (if subject exists in labeldata): auto& data = label_data_m[subject];
    std::cout << subject;
    drawProgressBar(subject);

    for (int i = 0, j = 0; i < all_data_m[subject].size(); ++i, j += 2) {
        std::cout << std::setw(alignment[j]) << label_data_m[subject][j] << std::setw(alignment[j + 1]) << "" << all_data_m[subject][i] << " "
                  << label_data_m[subject][j + 1];  // you never check for size of j, even though it should work with 'alignement'
        if (label_data_m[subject][j + 1] == "mins") {
            // function exists more than once, extract to utils.hpp or utils.cpp if it fits nowhere:)
            std::cout << std::setw(5) << "" << convertMinsToHrs(all_data_m[subject][i]) << " hrs";
        }
        std::cout << std::endl;  // '\n'
    }
    std::cout << std::endl;  // '\n'
}

void Data::viewAllData() {
    for (auto i : subjects_m) {  // auto is actually a string, not string&, so there is a copy in each loop, do auto& or const auto&
        viewData(i);
        std::cout << std::endl;  // '\n'
    }
}

void Data::printCategoriesToEdit(std::string subject) {
    // the same loop already exists in viewData, maybe combine into some:
    // void alignemntPrinter(subject, auto&& printfunc);
    std::vector<int> alignement = findOutAlignment(label_data_m[subject], 5);  // buffer=5 is never used in findoutalignment
    for (int i = 0, j = 0; i < all_data_m[subject].size(); ++i, j += 2) {
        std::cout << std::setw(2) << "(" << i << ") " << label_data_m[subject][j] << std::setw(alignement[j + 1]) << "" << all_data_m[subject][i]
                  << " " << label_data_m[subject][j + 1] << std::endl;
    }
}

void Data::editData(std::string subject) {
    std::cout << "\nPick a category to edit." << std::endl;
    printCategoriesToEdit(subject);
    int category = 0;
    std::cout << ">> ";
    std::cin >> category;
    std::cout << std::string(40, '-') << "\n";
    if (category >= all_data_m[subject].size()) {
        std::cout << "invalid category, please run the program again." << std::endl;  // '\n'
        return;
    }
    std::cout << "\nHave you finished work (0) or do you want to add work (1)?" << std::endl;  // '\n'
    int work = 0;
    std::cout << ">> ";
    std::cin >> work;  // explodes if you enter something other than an int, extract this into
    // template <typename T>
    // T getInput("User Promt here", std::istream& is = std::cin);
    // or
    // template <typename T>
    // void getInput("User Promt here", T& buffer); // for no allocations

    std::cout << std::string(40, '-') << "\n";

    int         amount = 0;
    int         value  = 0;
    std::string new_val;
    if (work == 0) {  // switch case, also early exit for fails if you do if else chains
        // so:
        // if (invalid) {
        //      error;
        //      return;
        // }
        //
        // if (case1) {
        //  handle1
        // } else if (case2) {
        //  handle2
        // } ...
        std::cout << "\nPlease enter the amount of work you finished" << std::endl;
        std::cout << ">> ";
        std::cin >> amount;
        std::cout << std::string(40, '-') << "\n";
        value = std::stoi(all_data_m[subject][category]) - amount;

        new_val                       = std::to_string(value);
        all_data_m[subject][category] = new_val;
    } else if (work == 1) {
        std::cout << "\nPlease enter the amount of work you want to add" << std::endl;
        std::cout << ">> ";
        std::cin >> amount;
        std::cout << std::string(40, '-') << "\n";
        value = std::stoi(all_data_m[subject][category]) + amount;

        new_val                       = std::to_string(value);
        all_data_m[subject][category] = new_val;
    }
    // throw error TODO
    else {
        std::cout << "invalid option, please run the program again." << std::endl;
        return;
    }

    // function is called editData, but it does:
    // io
    // logic
    // file io
    // can be split
    //
    // write to file
    std::ofstream fout(filepath_m);  // explodes or creates a new file if it does not exist
    for (auto i : all_data_m) {
        fout << i.first << ",";
        for (auto j : i.second) {
            fout << j << ",";
        }
        fout << "\n";
    }
    fout.close();

    std::cout << "CSV updated" << std::endl;
    viewData(subject);
}

// only the first two after the total are relevant,
void Data::drawProgressBar(std::string_view subject) {
    int size = subject.size();
    std::cout << std::setw(20 - size) << "";

    double total = std::stod(all_data_m[subject][0]);
    double done  = total;  // done = 0;
    for (int i = 1; i < all_data_m[subject].size(); ++i) {
        done -= std::stod(all_data_m[subject][i]);  // done += ...;
    }
    // done = total - done;

    double percent = (done / total) * 100;
    int    width   = 20;  // total width of the bar, already known, do static constexpr int width, compiler loves it
    int    pos     = percent / 100 * width;

    std::cout << "[";
    for (int i = 0; i < width; ++i) {
        if (i < pos)
            std::cout << "=";
        else
            std::cout << " ";
    }
    std::cout << "] " << int(percent) << " %\n";  // nice! \n
    std::cout << std::endl;                       // not nice! std::endl

    // does the same
    // std::cout << "[" << std::string(pos, "=") << std::string(width - pos, " ") << "] " << int(percent) << "%\n\n";
}
