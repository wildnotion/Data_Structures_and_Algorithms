//============================================================================
// Name        : ABCU_ComputerScienceDepartment_AdvisingAssistanceSoftware.cpp
// Author      : John Clark
// Version     : 1.0
// Copyright   : Copyright © 2023 SNHU COCE
// Description : For advisors at ABCU to help Computer Science students determine which courses must be taken to earn a 
//               Computer Science degree, excluding general education requirements. 
//============================================================================

#include <iostream> // Allows input to and output from the console 
#include <time.h> // Allows the measurement of time for loading, sorting, and displaying data
#include <fstream> // Allows files to be opened and closed
#include <sstream> // Allows the use of stringstream for string manipulation
#include <string> // Allows the use of pre-made string functions such as "getline"
#include <vector> // Allows the use of vectors and their pre-made functions
#include <iomanip> // Allows some formatting options such as setw for display

using namespace std; // Prevents the need for extraneous declarations (i.e., "std::") before many functions

struct CourseObject { // Defining what a CourseObject holds
    string courseNumber;
    string name;
    vector<string> prerequisites;
};

struct BSTNode { // Defining the internal structure for BinarySearchTree node
    CourseObject courseObject;
    BSTNode* left;
    BSTNode* right;

    BSTNode() { // Default constructor
        left = nullptr;
        right = nullptr;
    }

    BSTNode(CourseObject aCourseObject) : BSTNode() { // Initialize a BSTNode with a courseObject
        courseObject = aCourseObject;
    }
};

class BinarySearchTree { // Defining the functions and variables of a BinarySearchTree to be used in this program 
private:
    BSTNode* root;
    void addNode(BSTNode* node, CourseObject courseObject);
    void destroyTree(BSTNode* node);
    void inOrder(BSTNode* node);
    BSTNode* removeNode(BSTNode* node, string courseNumber);
    BSTNode* findMin(BSTNode* node);
    int Size(BSTNode* node);

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    int Size();
    void InOrder();
    void Insert(CourseObject courseObject);
    void Remove(string courseNumber);
    CourseObject Search(string courseNumber);
};

//============================================================================
// Global variables accessible by all functions
//============================================================================
BinarySearchTree* bst; // Define a BinarySearchTree to hold all course objects
string fileHeader; // Stores the first line of the file as a header to be used while printing but not stored in the BinarySearchTree
vector<string> allCourseNumbers; // Stores a list of all course numbers so that prerequisite errors may be found
clock_t ticks; // To measure the time of each specified function
int nodesSearched; // Keep track of amount of nodes searched

void displayCourse(CourseObject courseObject) { // Display individual CourseObject information to the console
    cout << setw(10) << left << courseObject.courseNumber << "|" << setw(35) << left << courseObject.name << "|";
    if (!courseObject.prerequisites.empty()) { // Displays any prerequisites held by the CourseObject
        bool firstPrerequisite = true;
        for (const string& prerequisite : courseObject.prerequisites) {
            if (!firstPrerequisite) {
                cout << "|";
            }
            cout << setw(14) << left << prerequisite;
            firstPrerequisite = false;
        }
    }
    else {
        cout << setw(14) << left << "None";
    }
    cout << endl;
}

BinarySearchTree::BinarySearchTree() { // Default constructor for a BinarySearchTree (Public)
    root = nullptr; // Initialize the root of the BinarySearchTree as nullptr
}

BinarySearchTree::~BinarySearchTree() { // Destructor for a BinarySearchTree used when delete is called (Public)
    destroyTree(root); // Call the function to recursively destroy the entire BinarySearchTree
}

int BinarySearchTree::Size() { // Returns the size of a BinarySearchTree (Public)
    return Size(root); // Call the function to calculate the size of the BinarySearchTree starting from the root
}

int BinarySearchTree::Size(BSTNode* node) { // Recursively returns the size of a BinarySearchTree  (Private)
    if (node == nullptr) { // If the current node is nullptr (empty), return 0
        return 0;
    }
    return 1 + Size(node->left) + Size(node->right); // Recursively calculate the size of the tree by counting all children starting at the root
}

void BinarySearchTree::InOrder() { // Traverse the BinarySearchTree in order (Public)
    inOrder(root); // Call the function for in-order traversal of the BinarySearchTree starting from the root
}

void BinarySearchTree::inOrder(BSTNode* node) { // Recursively call the function for in-order traversal of the BinarySearchTree starting from the root
    if (node != nullptr) {
        inOrder(node->left); // Recursively traverse the left subtree
        displayCourse(node->courseObject); // Display the courseObject at the passed in node
        inOrder(node->right); // Recursively traverse the right subtree
    }
}

void BinarySearchTree::Insert(CourseObject courseObject) { // Insert a CourseObject into the BinarySearchTree (Public)
    if (root == nullptr) { // If the tree is empty, create a new root node with the passed in courseObject
        root = new BSTNode(courseObject);
    }
    else {
        addNode(root, courseObject); // Otherwise, call a function to recursively add the courseObject as a node to the tree
    }
}

void BinarySearchTree::Remove(string courseNumber) { // Remove a courseObject from the BinarySearchTree (Public)
    root = removeNode(root, courseNumber); // Call a function to remove the node with the passed in courseNumber from the BinarySearchTree
}

CourseObject BinarySearchTree::Search(string courseNumber) { // Search for a courseObject
    BSTNode* currentNode = root; // Start the search from the root of the tree
    nodesSearched = 0;
    while (currentNode != nullptr) {
        if (currentNode->courseObject.courseNumber == courseNumber) { // If the courseNumber matches, return the found courseObject
            nodesSearched++;
            return currentNode->courseObject;
        }
        else if (courseNumber < currentNode->courseObject.courseNumber) { // Move to the left subtree if the passed in courseNumber is less than currentNode's courseNumber
            currentNode = currentNode->left;
            nodesSearched++;
        }
        else { // Move to the right subtree if the passed in courseNumber is greater than currentNode's courseNumber
            currentNode = currentNode->right;
            nodesSearched++;
        }
    }
    CourseObject courseObject; // If the passed in courseNumber is not found, create and return an empty courseObject
    return courseObject;
}

void BinarySearchTree::addNode(BSTNode* node, CourseObject courseObject) { // Recursively add a courseObject to an accurate node in the BinarySearchTree
    if (courseObject.courseNumber < node->courseObject.courseNumber) {
        if (node->left == nullptr) { // If the left child is empty, create a new left child node with the passed in courseObject
            node->left = new BSTNode(courseObject);
        }
        else {
            addNode(node->left, courseObject); // Otherwise, recursively add the passed in courseObject to the left subtree
        }
    }
    else {
        if (node->right == nullptr) { // If the right child is empty, create a new right child node with the passed in courseObject
            node->right = new BSTNode(courseObject);
        }
        else {
            addNode(node->right, courseObject); // Otherwise, recursively add the passed in courseObject to the right subtree
        }
    }
}

void BinarySearchTree::destroyTree(BSTNode* node) { // Recursively destroy the BinarySearchTree nodes
    if (node != nullptr) {
        destroyTree(node->left); // Recursively destroy the left subtree
        destroyTree(node->right); // Recursively destroy the right subtree
        delete node; // Delete the current node, which is the passed in root from the destructor
    }
}

BSTNode* BinarySearchTree::findMin(BSTNode* node) { // Find the node with the minimum value in the BinarySearchTree (included for modularity, but never explicitly called in this program)
    while (node->left != nullptr) { // Traverse to the left child until the minimum node is found
        node = node->left;
    }
    return node; // Return the minimum node
}

BSTNode* BinarySearchTree::removeNode(BSTNode* node, string courseNumber) { // Removes a node from the BinarySearchTree (included for modularity, but never explicitly called in this program)
    if (node == nullptr) { // If the node is nullptr, return it as there is nothing to remove
        return node;
    }
    if (courseNumber < node->courseObject.courseNumber) {
        node->left = removeNode(node->left, courseNumber); // Recursively traverse the left subtree
    }
    else if (courseNumber > node->courseObject.courseNumber) {
        node->right = removeNode(node->right, courseNumber); // Recursively traverse the right subtree
    }
    else {
        if (node->left == nullptr) { // If the node has no left child, replace it with the right child
            BSTNode* temp = node->right;
            delete node;
            return temp;
        }
        else if (node->right == nullptr) { // If the node has no right child, replace it with the left child
            BSTNode* temp = node->left;
            delete node;
            return temp;
        }
        BSTNode* temp = findMin(node->right); // Find the minimum node in the right subtree
        node->courseObject = temp->courseObject; // Replace the current node's courseObject with the minimum node's courseObject
        node->right = removeNode(node->right, temp->courseObject.courseNumber); // Recursively remove the minimum node
    }
    return node; // Return the modified node to maintain the tree's structure
}

int checkForFileFormatErrors(const CourseObject& newCourse, const vector<string>& availableCourses) { // Checks the CourseObject variables for errors, including prerequisites that are not found in available courses
    if (newCourse.courseNumber.empty()) {
        return 1; // Error code for blank course number
    }
    if (newCourse.name.empty()) {
        return 2; // Error code for blank course name
    }
    for (const string& prerequisite : newCourse.prerequisites) {
        bool foundPrerequisite = false;
        for (const string& availableCourse : availableCourses) {
            if (prerequisite == newCourse.courseNumber) {
                return 3; // Error code for prerequisite equalling its own course number
            }
            if (prerequisite == availableCourse) {
                foundPrerequisite = true;
                break;
            }
        }
        if (!foundPrerequisite) {
            return 4; // Error code for missing prerequisite in available courses
        }
    }
    return 0; // No errors found
}

int parseCourseLine(const string& line, CourseObject& newCourse, const vector<string>& availableCourses) { // Parses each line from the input file to create CourseObjects and check for file format errors
    size_t commaLoc = line.find(','); // Sets the location of the first found comma
    if (commaLoc != string::npos) { // If the comma location is still in the provide line
        newCourse.courseNumber = line.substr(0, commaLoc); // Sets the courseNumber to the string found before the first comma
        string restOfLine = line.substr(commaLoc + 1); // Checks rest of the line
        commaLoc = restOfLine.find(','); // Sets the location of the second found comma
        if (commaLoc != string::npos) { // If the comma location is still in the provide line
            newCourse.name = restOfLine.substr(0, commaLoc); // Sets the course name to the string found before the second comma
            restOfLine = restOfLine.substr(commaLoc + 1); // Checks rest of the line
            while (!restOfLine.empty()) { // If there are more commas in the line
                commaLoc = restOfLine.find(','); // Sets the location of the next found comma
                string prerequisite = restOfLine.substr(0, commaLoc); // Sets a prerequisite to the string found before the next comma
                if (!prerequisite.empty() && prerequisite != " ") { // If the prerequisite is not empty or blank
                    newCourse.prerequisites.push_back(prerequisite); // Adds the prerequisite to the CourseObjects prerequisite vector
                }
                restOfLine = restOfLine.substr(commaLoc + 1); // Checks rest of the line
            }
        }
        else {
            newCourse.name = restOfLine; // Rest of the line is the course name
        }
    }
    int error = checkForFileFormatErrors(newCourse, availableCourses); // Checks for errors in the new course object, including checking prerequisites against the available course list
    if (error != 0) { // If an error is found
        return error; // Return the error code
    }
    return 0; // If no errors are found, the new course object is okay to be added to a node
}

void printHeader(const string& line) { // Displays the first line of the provided file as a header
    istringstream lineStream(line); // Create a string stream to process the extracted header line from the input file
    string item; // Variable to parse info from extracted header line
    vector<string> items; // Variable to store those items
    while (getline(lineStream, item, ',')) { // Extracts using commas as delimiters
        items.push_back(item); // Appends the items vector
    }
    cout << setw(10) << left << items[0] << "|" << setw(35) << left << items[1] << "|";
    for (size_t i = 2; i < items.size(); ++i) {
        cout << setw(14) << left << items[i] << "|";
    }
    cout << endl;
}

void displayAllCoursesAlphanumericOrder(BinarySearchTree* bst) {
    ticks = clock(); // Starting clock ticks for timing how long this function takes
    if (bst != nullptr) {
        cout << "Courses Listed by Alphanumeric Order of Course Number:" << endl;
        printHeader(fileHeader); // Calls for the header to be displayed
        bst->InOrder(); // Calls the inOrder function in the BinarySearchTree class to display each CourseObject by ascending alphanumeric order based on course numbers
        cout << endl;
    } else {
        cout << "There is no data in the binary search tree.\n" << endl; // If the BinarySearchTree has yet to have data added to it
        return;
    }
    ticks = clock() - ticks; // End of function clock ticks minus starting clock ticks
    cout << "time: " << ticks << " clock ticks" << endl; // Display elapsed time results
    cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
    cout << "Displayed " << bst->Size() << " courses from the binary search tree.\n" << endl;
}

void displaySpecificCourse(BinarySearchTree* bst) { // Displays information about a specific course based on user input
    if (bst != nullptr) { // Check if the BinarySearchTree is not empty
        string courseNumber, upperCourse;
        cout << "Enter a course number to search for: ";
        getline(cin, courseNumber);
        ticks = clock(); // Starting clock ticks
        upperCourse = courseNumber;
        for (char& c : upperCourse) { // If a user enters any lowercase letters in their search, adjusts them to uppercase for accurate and easier searches
            c = toupper(c);
        }
        cout << endl;
        CourseObject foundCourse = bst->Search(upperCourse); // Calls the search function to find a fully uppercased course number
        if (!foundCourse.courseNumber.empty()) { // If the search was successful, display associated course object information
            cout << setw(15) << left << "Course Number: " << foundCourse.courseNumber << endl;
            cout << "Name" << setw(11) << right << ": " << foundCourse.name << endl;
            cout << setw(15) << left << "Prerequisites: ";
            bool foundPrerequisites = false;
            for (const string& prerequisite : foundCourse.prerequisites) {
                if (foundPrerequisites == false) {
                    cout << prerequisite;
                    foundPrerequisites = true;
                }
                else {
                    cout << ", " << prerequisite;
                }
            }
            if (foundPrerequisites == false) {
                cout << "None";
            }
            cout << endl << endl;
        } else {
            cout << "Course Number: \"" << courseNumber << "\" was not found\n" << endl; // Else display that the user entered course was not found
        }
        ticks = clock() - ticks; // End of function clock ticks minus starting clock ticks
        cout << "time: " << ticks << " clock ticks" << endl; // Display elapsed time results
        cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
        cout << "Searched through " << nodesSearched << " of " << bst->Size() << " courses in the binary search tree.\n" << endl;
    } else {
        cout << "There is no data in the binary search tree.\n" << endl; // If the BinarySearchTree has yet to have data added to it
        return;
    }
}

vector<string> extractAllCourseNumbers(ifstream& inputFile) { // Function to create a list of all course numberss found in the input file based on the first element of each line
    string currentLine;
    getline(inputFile, currentLine); // Skip the first line which is a header, that is handles elsewhere for display
    while (getline(inputFile, currentLine)) { // Extract the course numbers from the remaining lines of the input file
        istringstream lineStream(currentLine); // Create a string stream to process the current line of the input file
        string courseNumber;
        getline(lineStream, courseNumber, ','); // Get the course number from the line until the first comma
        allCourseNumbers.push_back(courseNumber); // Add the course number to the vector
    }
    return allCourseNumbers; // Return the vector containing all extracted course numbers to be used as a prerequisite comparison
}

CourseObject createNewCourseObject (const string& line, const vector<string>& availableCourses, int lineCount) { // Creates a new CourseObject based on the provided line and available courses
    CourseObject newCourse; // Creates a new CourseObject to hold the parsed data
    int error = parseCourseLine(line, newCourse, availableCourses); // Parses the provided line and stores the results in the new CourseObject
    if (error != 0) { // If an error occurred during parsing
        if (error == 1) {
            cout << "Missing course number on line " << lineCount << " of the provided file.\n" << endl;
        }
        if (error == 2) {
            cout << "Missing course name on line " << lineCount << " of the provided file.\n" << endl;
        }
        if (error == 3) {
            cout << "The course on line " << lineCount << " has itself as a prerequisite.\n" << endl;
        }
        if (error == 4) {
            cout << "There was no available course matching the prerequisite\nfound on line " << lineCount << " of the provided file.\n" << endl;
        }
        if (error != 1 && error != 2 && error != 3) {
            cout << "An unknown error was found on line " << lineCount << " of the provided file." << endl; // Should not be called in the scope of this program
        }
        delete bst; // Delete BinarySearchTree and free its memory
        bst = nullptr;
        return newCourse; // Returns an empty course object because an error occurred
    }
    return newCourse; // Returns a completed course object
}

void processRequestedFile(string filePath) {
    ticks = clock(); // Starting clock ticks
    ifstream inputFile(filePath); // Tries to open the file at the given filePath
    if (inputFile.is_open()) { // If the file at the given filePath was successfully opened
        cout << "\nSuccessfully opened the file at \"" << filePath << "\".\n" << endl;
        allCourseNumbers = extractAllCourseNumbers(inputFile); // Make a list of every course number, which is the first element of each line (i.e., in a CSV before the first comma on each line)
        inputFile.close(); // Close the file to reset the ifstream, otherwise following calls to the input file will read start from its end, which is empty
        inputFile.open(filePath); // Reopens the input file so it may be used again
        bst = new BinarySearchTree(); // Initializes a new BinarySearchTree to hold nodes with stored course objects
        string currentLine;
        getline(inputFile, fileHeader); // This assumes the first line of the CSV is a descriptive header, and does not include it in the array
        int lineCount = 2; // Excludes the header so that errors may be pinpointed to specific lines of the input file
        while (getline(inputFile, currentLine)) { // Read and process each line in the file
            CourseObject newCourse = createNewCourseObject(currentLine, allCourseNumbers, lineCount); // Create new course objects to be stored in BinarySearchTree nodes
            if (bst != nullptr) { // If an error is found and the BinarySearchTree is deleted, a new node is not created and this function ends, returning the user to the main menu with an error message
                bst->Insert(newCourse);
                lineCount++; // Increments a line count that can pinpoint where in the input file an error may be found
            }
            else {
                return; // Exits the function as an error was found in the file format
            }
        }
        inputFile.close(); // Closes the input file
        ticks = clock() - ticks; // End of function clock ticks minus starting clock ticks
        cout << "time: " << ticks << " clock ticks" << endl; // Display elapsed time results
        cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
        cout << "Loaded " << bst->Size() << " courses into the binary search tree.\n" << endl;
    }
    else { // If the file at the given filePath was NOT successfully opened
        cout << "\nFailed to open the file at \"" << filePath << "\".\n" << endl;
    }
}

int main() { // The main() function responsible for running the program
    string filePath, courseNumber, confirmation, tempChoice;
    int choice = 0, choice2;
    cout << "Welcome to ABC University's Computer Science\nDepartment's Advising Assistance Software!\n" << endl;
    while (choice != 4) { // Displays a menu as long as the user does not choose to exit
        cout << "Main Menu:" << endl;
        cout << "  1. Load Course Information into a Binary Search Tree" << endl;
        cout << "  2. Print Course List in Alphanumeric Order" << endl;
        cout << "  3. Print Specific Course Information" << endl;
        cout << "  4. Exit the Program" << endl;
        cout << "Please enter a menu selection: ";
        if (!(getline(cin, tempChoice)) || (tempChoice != "1" && tempChoice != "2" && tempChoice != "3" && tempChoice != "4")) { // Check to see if the user entered a valid choice, and if not sets the choice to 0
            cout << endl << "\"" << tempChoice << "\" is an invalid menu selection." << endl;
            cout << "Please enter a valid menu selection." << endl;
            cout << "(i.e., '1', '2', '3', or '4'.)\n" << endl;
            choice = 0; // Sets choice to 0 to repeat the menu options for the while loop at the switch(0)
        }
        else {
            cout << endl;
            choice = stoi(tempChoice); // Sets choice to the integer the user selected for tempChoice
        }
        switch (choice) {
        case 1:
            cout << "Please provide a file path for the file you wish to load.\nExample = C:\\Users\\John\\Desktop\\ABCU_CompSciData.csv\nFile Path: ";
            getline(cin, filePath); // filePath = "C:/Users/John/Desktop/ABCU_CompSciData.csv" was used for testing and required forward slashes for accessing the file
            processRequestedFile(filePath);
            break;
        case 2:
            displayAllCoursesAlphanumericOrder(bst);
            break;
        case 3:
            displaySpecificCourse(bst);
            break;
        }
    }
    delete bst; // Delete BinarySearchTree and free its memory    
    cout << "Thank you for using the ABC University's Computer\nScience Department's Advising Assistance Software!" << endl;
    return 0;
}