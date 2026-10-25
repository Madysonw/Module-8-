// ProjectTwo.cpp
// CS 300 Project Two - ABCU Advising Assistance Program
// Reads a CSV file, stores courses in a sorted map, and provides a course planner menu.

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Represents a course and the course numbers of its prerequisites.
struct Course {
    string number;
    string title;
    vector<string> prerequisites;
};

// A map keeps courses sorted alphanumerically by course number.
using CourseCatalog = map<string, Course>;

// Remove leading and trailing whitespace (including Windows line endings).
string trim(const string& value) {
    const string whitespace = " \t\r\n";
    size_t first = value.find_first_not_of(whitespace);
    if (first == string::npos) {
        return "";
    }
    size_t last = value.find_last_not_of(whitespace);
    return value.substr(first, last - first + 1);
}

// Normalize course numbers so searches are case-insensitive.
string normalizeCourseNumber(string number) {
    number = trim(number);
    transform(number.begin(), number.end(), number.begin(),
              [](unsigned char ch) { return static_cast<char>(toupper(ch)); });
    return number;
}

// Read a CSV file into a temporary catalog, then replace the current catalog
// only if the file and all course references are valid.
bool loadCourses(const string& filename, CourseCatalog& catalog) {
    ifstream input(filename);
    if (!input.is_open()) {
        cout << "Error: Could not open file '" << filename << "'.\n";
        return false;
    }

    CourseCatalog newCatalog;
    string line;
    size_t lineNumber = 0;

    while (getline(input, line)) {
        ++lineNumber;
        if (trim(line).empty()) {
            continue;
        }

        stringstream stream(line);
        vector<string> fields;
        string field;
        while (getline(stream, field, ',')) {
            fields.push_back(trim(field));
        }

        if (fields.size() < 2 || fields[0].empty() || fields[1].empty()) {
            cout << "Error: Invalid course data on line " << lineNumber << ".\n";
            return false;
        }

        Course course;
        course.number = normalizeCourseNumber(fields[0]);
        course.title = fields[1];
        for (size_t index = 2; index < fields.size(); ++index) {
            if (!fields[index].empty()) {
                course.prerequisites.push_back(normalizeCourseNumber(fields[index]));
            }
        }

        if (newCatalog.find(course.number) != newCatalog.end()) {
            cout << "Error: Duplicate course " << course.number
                 << " on line " << lineNumber << ".\n";
            return false;
        }
        newCatalog[course.number] = course;
    }

    if (input.bad() || newCatalog.empty()) {
        cout << "Error: File could not be read or contains no courses.\n";
        return false;
    }

    // Prerequisites must refer to a course in the loaded file.
    for (const auto& entry : newCatalog) {
        for (const string& prerequisite : entry.second.prerequisites) {
            if (newCatalog.find(prerequisite) == newCatalog.end()) {
                cout << "Error: Unknown prerequisite " << prerequisite
                     << " for " << entry.first << ".\n";
                return false;
            }
        }
    }

    catalog.swap(newCatalog);
    cout << "Successfully loaded " << catalog.size() << " courses.\n";
    return true;
}

// The map iterates in ascending order, so no additional sort is needed.
void printCourseList(const CourseCatalog& catalog) {
    cout << "\nHere is a sample schedule:\n\n";
    for (const auto& entry : catalog) {
        cout << entry.second.number << ", " << entry.second.title << '\n';
    }
    cout << '\n';
}

// Print the selected course, including prerequisite numbers and titles.
void printCourse(const CourseCatalog& catalog, const string& courseNumber) {
    const string key = normalizeCourseNumber(courseNumber);
    auto found = catalog.find(key);
    if (found == catalog.end()) {
        cout << "Course " << key << " was not found.\n";
        return;
    }

    const Course& course = found->second;
    cout << course.number << ", " << course.title << '\n';
    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None\n";
    } else {
        cout << "Prerequisites:\n";
        for (const string& number : course.prerequisites) {
            const Course& prerequisite = catalog.at(number);
            cout << "  " << prerequisite.number << ", " << prerequisite.title << '\n';
        }
    }
    cout << '\n';
}

void printMenu() {
    cout << "\n  1. Load Data Structure.\n"
         << "  2. Print Course List.\n"
         << "  3. Print Course.\n"
         << "  9. Exit\n\n"
         << "What would you like to do? ";
}

int main() {
    CourseCatalog catalog;
    string input;

    cout << "Welcome to the course planner.\n";

    while (true) {
        printMenu();
        if (!getline(cin, input)) {
            cout << "\nThank you for using the course planner!\n";
            break;
        }
        input = trim(input);

        if (input == "1") {
            cout << "Enter the course data file name: ";
            string filename;
            if (!getline(cin, filename)) {
                cout << "\nInput ended.\n";
                break;
            }
            filename = trim(filename);
            if (filename.empty()) {
                cout << "Error: File name cannot be empty.\n";
            } else {
                loadCourses(filename, catalog);
            }
        } else if (input == "2") {
            if (catalog.empty()) {
                cout << "Please load the course data first (option 1).\n";
            } else {
                printCourseList(catalog);
            }
        } else if (input == "3") {
            if (catalog.empty()) {
                cout << "Please load the course data first (option 1).\n";
            } else {
                cout << "What course do you want to know about? ";
                string courseNumber;
                if (!getline(cin, courseNumber)) {
                    cout << "\nInput ended.\n";
                    break;
                }
                printCourse(catalog, courseNumber);
            }
        } else if (input == "9") {
            cout << "Thank you for using the course planner!\n";
            break;
        } else {
            cout << input << " is not a valid option.\n";
        }
    }

    return 0;
}
