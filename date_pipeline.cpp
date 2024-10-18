//program which stimulates the analysis of the dump count based on the TAB and TCH[Topographic Correction High] files.

// include the necessary header files
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <cstdio>

using namespace std;

//define and declare the global vairbales
int pitch_pos_dump = 0;
int pitch_neg_dump = 0;
int yaw_pos_dump = 0;
int yaw_neg_dump = 0;
int day_ct = 0;
int s_no = 1;

// clears the data of the file
void clearFile(const string& fileName) {
    // Open the file in truncation mode to clear its content
    std::ofstream file(fileName, std::ios::trunc);
    if (!file) {
        std::cerr << "Error: Unable to clear file: " << fileName << std::endl;
        exit(0);
    }
    file.close(); // Close the file
}

// initialises the output file
void initOutputFile(const std::string& fileName) {
    std::ofstream f1(fileName, std::ios::trunc);  // Open in truncation mode to clear content or create the file
    if (!f1) {
        std::cerr << "Error: Unable to create or open file: " << fileName << std::endl;
        exit(0);
    }
    f1 << " _______________________________________" << std::endl;
    f1 << "| Sn|\tDate\t|\tPitch\t|\tYaw    \t|" << std::endl;
    f1 << "|   |\t    \t|-----------|-----------|" << std::endl;
    f1 << "|   |\t    \t| +ve | -ve | +ve | -ve |" << std::endl;
    f1 << "|---|-----------|-----|-----|-----|-----|" << std::endl;
    f1.close();
}
// returns the next date string
string date_gen(string present) {
    int day, month, year;
    stringstream ss(present);
    string word;
    vector<string> vec;
    while(getline(ss, word, '-'))
        vec.push_back(word);
    unordered_map<string,int> map = {
        {"JAN", 1}, {"FEB", 2}, {"MAR", 3}, {"APR", 4},
        {"MAY", 5}, {"JUN", 6}, {"JUL", 7}, {"AUG", 8},
        {"SEP", 9}, {"OCT", 10}, {"NOV", 11}, {"DEC", 12}
    };
    unordered_map<int,string> monthMap = {
        {1, "JAN"}, {2, "FEB"}, {3, "MAR"}, {4, "APR"},
        {5, "MAY"}, {6, "JUN"}, {7, "JUL"}, {8, "AUG"},
        {9, "SEP"}, {10, "OCT"}, {11, "NOV"}, {12, "DEC"}
    };
    day = stoi(vec[0]);
    month = map[vec[1]];
    year = stoi(vec[2]);
    
    // Calculate the next day
    int nextDay = day + 1;
    int nextMonth = month;
    int nextYear = year;

    // Handle month-end and year-end logic
    int daysInMonth = 31;

    if (nextMonth == 2) {
        // February logic
        daysInMonth = ( (nextYear % 4 == 0 && nextYear % 100 != 0) || nextYear % 400 == 0 ) ? 29 : 28;
    } 
    else if (nextMonth == 4 || nextMonth == 6 || nextMonth == 9 || nextMonth == 11) {
        daysInMonth = 30;
    }
    if (nextDay > daysInMonth) {
        nextDay = 1;
        nextMonth++;
        if (nextMonth > 12) {
            nextMonth = 1;
            nextYear++;
        }
    }
   string res="";
    if(nextDay<10)
        res = "0"+to_string(nextDay) + "-" + monthMap[nextMonth] + "-" + to_string(nextYear);
    else  
         res = to_string(nextDay) + "-" + monthMap[nextMonth] + "-" + to_string(nextYear);
    
    //return the res string
    return res;
}

// filters the tch file based on the dump command
void cmd_filter(string inputFile) {
    // Patterns to search
    string pitchEN = "";
    string pitchDS = "";
    string yawEN = "";
    string yawDS = "";

    //open the configure file to get the patterns
    ifstream con("config.txt");
    if(!con) {
        cerr<< "Unable to open file: config.txt"<<endl;
        exit(0);
    }
    getline(con,pitchEN);
    getline(con,pitchDS);
    getline(con,yawEN);
    getline(con,yawDS);

    // Open the input file
    ifstream inFile(inputFile);
    // handle the error
    if (!inFile) {
        cerr << "Unable to open file: inside cmg filter" << (string)inputFile << endl;
        exit(0);
    }

    // Open the output file
    ofstream op_pitch("pitch_filter.txt",ios::trunc);
    ofstream op_yaw("yaw_filter.txt", ios::trunc);
    // handle the error
    if (!op_pitch || !op_yaw) {
        cerr << "Unable to create file: pitch or yaw filter file" << endl;
        exit(0);
    }

    string line;
    // Read the input file line by line
    while (getline(inFile, line)) {
        // Search for the patterns in each line
        if (line.find(pitchEN) != string::npos || line.find(pitchDS) != string::npos) {
            // Write the matching line to the output file
            op_pitch << line << endl;
        }
        if(line.find(yawEN) != string::npos || line.find(yawDS) != string::npos) {
            op_yaw << line <<endl;
        }
    }

    // Close the files
    inFile.close();
    op_pitch.close();
    op_yaw.close();

    cout << "\n.....First Phase complete... CMD filtered..." << endl;
}

// Function to split a line into fields using whitespace as the delimiter
std::vector<std::string> splitLine(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> fields;
    std::string field;
    while (iss >> field) {
        fields.push_back(field);
    }
    return fields;
}

// Function to compare two times (HH:MM:SS format)
bool isTimeInRange(const std::string& time, const std::string& startTime, const std::string& endTime) {
    return time >= startTime && time <= endTime;
}

// output generating function
void dump_ct(string data_filter, int flag, int s_no,string date) {

    // re-initialise the variables
    pitch_pos_dump = 0;
    pitch_neg_dump = 0;
    yaw_pos_dump = 0;
    yaw_neg_dump = 0;
    // the output file
    ofstream out("output.txt", ios::app);
    ofstream pplot("pitch_plot.txt", ios::app);
    ofstream yplot("yaw_plot.txt", ios::app);
    if(!out) {
        cerr<<"Unable to create file..inside the dump_ct"<<endl;
        exit(0);
    }
    if(!pplot) {
        cerr<<"Unable to create plot file"<<endl;
    }
    if(!pplot) {
        cerr<<"Unable to create plot file"<<endl;
    }
    // the input file
    ifstream in(data_filter);
    if(!in) {
        cerr<<"Unable to open file..inside dump_ct"<<data_filter<<endl;
        exit(0);
    }

    if(in.peek() != std::ifstream::traits_type::eof()) {
        // calculate the pitch var
        if(flag == 0) {

            vector<pair<int, int>> status;
    
            if(in.good()) {

                std::string line;
                while (getline(in, line)) {
                    auto fields = splitLine(line); // Assuming splitLine correctly splits the line
            
                    // // Avoid duplicates in date
                    // if (find(date.begin(), date.end(), fields[0]) == date.end())
                    //     date.emplace_back(fields[0]);
                
                    // Store pairs of values (assuming the correct indices are being used)
                    status.emplace_back(make_pair(stoi(fields[2]), stoi(fields[fields.size() - 2])));
                }
        
        
                // Traverse the vector to find dump counts
                for (size_t i = 0; i < status.size() - 1; i++) {
                    if (abs(status[i + 1].first - status[i].first) > 200 && status.back().second < status.front().second)
                        pitch_pos_dump++;
                    else if(abs(status[i + 1].first - status[i].first) > 200 && status.back().second > status.front().second)
                        pitch_neg_dump++;
                    // cout<<"Dump for the day: "<<ct<<" "<<dump<<endl;
                }
            
            }
        }
        if(flag == 1) {

            vector<pair<int, int>> status;
    
            if(in.good()) {

                std::string line;
                while (getline(in, line)) {
                    auto fields = splitLine(line); // Assuming splitLine correctly splits the line
            
                    // // Avoid duplicates in date
                    // if (find(date.begin(), date.end(), fields[0]) == date.end())
                    //     date.emplace_back(fields[0]);
                
                    // Store pairs of values (assuming the correct indices are being used)
                    status.emplace_back(make_pair(stoi(fields[2]), stoi(fields[fields.size() - 1])));
                }
        
        
               // Traverse the vector to find dump counts
                for (size_t i = 0; i < status.size() - 1; i++) {
                    if (abs(status[i + 1].first - status[i].first) > 200 && status.back().second < status.front().second)
                        yaw_pos_dump++;
                    else if(abs(status[i + 1].first - status[i].first) > 200 && status.back().second > status.front().second)  
                        yaw_neg_dump++;
                    // cout<<"Dump for the day: "<<ct<<" "<<dump<<endl;
                }       
            } 
        }
    }
        out<<"| "<<s_no<<"\t|"<<date<<"|  "<<pitch_pos_dump<<"  |  "<<pitch_neg_dump<<"  |  "<<yaw_pos_dump<<"  |  "<<yaw_neg_dump<<"  |"<<endl;
        out << " ---------------------------------------" << std::endl;
        pplot<<date<<"\t"<<pitch_pos_dump<<"\t"<<pitch_neg_dump<<endl;
        yplot<<date<<"\t"<<yaw_pos_dump<<"\t"<<yaw_neg_dump<<endl;
        out.close();
        in.close();
        pplot.close();
        yplot.close();
}
// filters based on start and end time
void data_filter(string inputFile,int s_no,string date) {
    
    bool flag = -1; // 0 -> pitch 1 -> yaw
    string data_filter = "date_filter.txt";
    // Open filtered.txt to find the start and end times
    ifstream pitch("pitch_filter.txt");
    ifstream yaw("yaw_filter.txt");
    ifstream tab(inputFile);
    if (!pitch) {
        std::cerr << "Unable to open file: inside date_filter pitch"<< std::endl;
        exit(0);
    }
    if (!yaw) {
        std::cerr << "Unable to open file: inside date_filter yaw"<< std::endl;
        exit(0);
    }
    if (!tab) {
        std::cerr << "Unable to open file: inside date_filter tab"<< std::endl;
        exit(0);
    }
    if(pitch.peek() == std::ifstream::traits_type::eof() && yaw.peek() == std::ifstream::traits_type::eof())
        dump_ct(data_filter,flag,s_no,date);
    // first filter the pitch data
    else if(pitch.peek() != std::ifstream::traits_type::eof()) {

        std::string line1, line2;
        std::string startTime, endTime;
        std::ofstream out("date_filter.txt", std::ios::trunc);
        //declare the flag
        flag = 0;
        
        if (!out) {
            std::cerr << "Unable to create file: "<< std::endl;
            exit(0);
        }

        // Read two lines (enable and disable) at a time
        while (std::getline(pitch, line1) && std::getline(pitch, line2)) {
            std::vector<std::string> fields1 = splitLine(line1);
            std::vector<std::string> fields2 = splitLine(line2);

            if (fields1.size() >= 5 && fields2.size() >= 5) {
                // Extract start and end times from the two lines
                startTime = fields1[fields1.size() - 2]; // Start time from "enable" line
                endTime = fields2[fields2.size() - 2];   // End time from "disable" line

                // Debugging: Print the extracted start and end times for each event
                std::cout << "Start Time: " << startTime << ", End Time: " << endTime << std::endl;
                string line;
                // Process tab.txt and copy matching lines to filtered2.txt
                getline(tab,line);
                while (getline(tab, line)) {
                    std::vector<std::string> fieldsTab = splitLine(line);

                    if (fieldsTab.size() >= 2) {
                        std::string time = fieldsTab[1];  // Time field from tab.txt

                        // Check if the time falls within the range [startTime, endTime]
                        if (isTimeInRange(time, startTime, endTime)) {
                            out << line << std::endl;  // Write the matching line to filtered2.txt
                        }
                    }
                }
                dump_ct(data_filter,flag,s_no,date);
                // Reset the tab file stream to the beginning for the next event
                const string s = "date_filter.txt";
                clearFile(s);  // Clear any error flags
                tab.seekg(0); // Rewind the file to the beginning
            }
        }

        // Close the input and output files
        pitch.close();
        tab.close();
        out.close();
        
    }
    else {

        std::string line1, line2;
        std::string startTime, endTime;
        std::ofstream out("date_filter.txt", std::ios::trunc);
        //declare the flag
        flag = 1;

        if (!out) {
            std::cerr << "Unable to create file: "<< std::endl;
            exit(0);
        }
        // Read two lines (enable and disable) at a time
        while (std::getline(yaw, line1) && std::getline(yaw, line2)) {
            std::vector<std::string> fields1 = splitLine(line1);
            std::vector<std::string> fields2 = splitLine(line2);

            if (fields1.size() >= 5 && fields2.size() >= 5) {
                // Extract start and end times from the two lines
                startTime = fields1[fields1.size() - 2]; // Start time from "enable" line
                endTime = fields2[fields2.size() - 2];   // End time from "disable" line

                // Debugging: Print the extracted start and end times for each event
                std::cout << "Start Time: " << startTime << ", End Time: " << endTime << std::endl;
                string line;
                // Process tab.txt and copy matching lines to filtered2.txt
                getline(tab,line);
                while (getline(tab, line)) {
                    std::vector<std::string> fieldsTab = splitLine(line);

                    if (fieldsTab.size() >= 2) {
                        std::string time = fieldsTab[1];  // Time field from tab.txt

                        // Check if the time falls within the range [startTime, endTime]
                        if (isTimeInRange(time, startTime, endTime)) {
                            out << line << std::endl;  // Write the matching line to filtered2.txt
                        }
                    }
                }
                dump_ct(data_filter,flag,s_no,date);
                // Reset the tab file stream to the beginning for the next event
                const string s = "date_filter.txt";
                clearFile(s);  // Clear any error flags
                tab.seekg(0); // Rewind the file to the beginning
            }
        }

        // Close the input and output files
        yaw.close();
        tab.close();
        out.close();
    }
    std::cout << "\n...Second Phase complete. Data appended to the output.txt..." << std::endl;
}

// Driver function
int main(){
    // declare variables of local scope
    const string t = "output.txt";
    //initialise the output file
    initOutputFile(t);

    string input_date;
    cout<<"Enter the date of initiation (DD:MMM:YY) : ";
    getline(cin,input_date);
    cout<<"Enter the up-limiter: ";
    cin>>day_ct;
    // check for invalid input
    if(day_ct <=0 ){
        cerr<<"Invalid input error.."<<endl;
        exit(0);
    }
    // declare file pointers for checking later
    ifstream f1("pitch_filter.txt");
    ifstream f2("yaw_filter.txt");

    if(!f1.is_open() || !f2.is_open()) {
        cerr << "Error opening file" << endl;
        exit(0);
    }

    // enter into the loop
    for (int i = 0; i < day_ct; i++) {
        if (i == 0) {
            cout<<"DAY: "<<s_no<<endl;
            string input_tch = input_date + "_tch.txt";
            cmd_filter(input_tch);
            
            string input_tab = input_date + "_tab.txt";
            data_filter(input_tab,s_no++,input_date);
            
        }
        else {
            cout<<"DAY: "<<s_no<<endl;
            string next_date = date_gen(input_date);
            input_date = next_date;
            string input_tch = input_date + "_tch.txt";
            cmd_filter(input_tch);
            
            string input_tab = input_date + "_tab.txt";
            data_filter(input_tab,s_no++,input_date);
            
        }
    }
    // end the program as it done
    cout<<"Execution finished..Check output .txt for results.."<<endl;
    return 0;
}