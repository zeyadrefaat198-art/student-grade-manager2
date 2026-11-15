
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

struct Student {
  int id;
  string name;
  double grades[3];
  double total;
  double average;
  string status;
};

vector<Student> students;
int nextId = 1;

const string FILE_NAME = "students.txt";

void calculate(Student &s) {

  s.total = 0;
  for (int i = 0; i < 3; i++)
    s.total += s.grades[i];

  s.average = s.total / 3.0;
  s.status = (s.average >= 50) ? "Passed" : "Failed";
}

bool saveData() {
  ofstream file(FILE_NAME);

  if (!file.is_open()) {
    cout << "Error: Could not save data.\n";
    return false;
  }

  for (const Student &s : students) {
    file << s.id << "\n";
    file << s.name << "\n";

    for (int i = 0; i < 3; i++) {
      file << s.grades[i] << " ";
    }

    file << "\n";
  }

  file.close();

  if (file.fail()) {
    cout << "Error: Data could not be saved completely.\n";
    return false;
  }

  return true;
}
bool loadData() {
  ifstream file(FILE_NAME);

  if (!file.is_open()) {
    return false;
  }

  vector<Student> loadedStudents;
  Student s;
  int maxId = 0;

  while (true) {
    if (!(file >> s.id)) {
      if (file.eof()) {
        break;
      }

      cout << "Warning: Invalid student ID found.\n";
      return false;
    }

    if (s.id <= 0) {
      cout << "Warning: Invalid student ID found.\n";
      return false;
    }

    file.ignore(numeric_limits<streamsize>::max(), '\n');

    if (!getline(file, s.name) || s.name.empty()) {
      cout << "Warning: Invalid student name found.\n";
      return false;
    }

    for (int i = 0; i < 3; i++) {
      if (!(file >> s.grades[i]) || s.grades[i] < 0 || s.grades[i] > 100) {
        cout << "Warning: Invalid grades found.\n";
        return false;
      }
    }

    file.ignore(numeric_limits<streamsize>::max(), '\n');

    calculate(s);
    loadedStudents.push_back(s);

    if (s.id > maxId) {
      maxId = s.id;
    }
  }

  if (file.bad()) {
    return false;
  }

  students = loadedStudents;
  nextId = maxId + 1;

  return true;
}

int getInteger(string message) {
  int value;

  while (true) {
    cout << message;

    if (cin >> value)
      return value;

    cout << "Invalid input. Try again.\n";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
  }
}

double getGrade(string message) {
  double grade;

  while (true) {
    cout << message;

    if (cin >> grade && grade >= 0 && grade <= 100)
      return grade;

    cout << "Enter a grade between 0 and 100.\n";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
  }
}

int findStudent(int id) {
  for (int i = 0; i < (int)students.size(); i++)
    if (students[i].id == id)
      return i;

  return -1;
}

void addStudent() {
  Student s;
  s.id = nextId++;

  cin.ignore(numeric_limits<streamsize>::max(), '\n');

  cout << "Enter student name: ";
  getline(cin, s.name);

  for (int i = 0; i < 3; i++)
    s.grades[i] =
        getGrade("Enter grade for subject " + to_string(i + 1) + ": ");

  calculate(s);
  students.push_back(s);
  if (saveData()) {
    cout << "\nStudent added successfully!\n";
    cout << "Student ID: " << s.id << "\n";
  } else {
    students.pop_back();
    nextId--;
    cout << "\nFailed to save student data. Addition cancelled.\n";
  }
}

void displayStudent(const Student &s) {
  cout << "\n-----------------------------\n";
  cout << "ID: " << s.id << "\n";
  cout << "Name: " << s.name << "\n";

  for (int i = 0; i < 3; i++)
    cout << "Subject " << i + 1 << ": " << s.grades[i] << "\n";

  cout << "Total: " << s.total << "\n";
  cout << "Average: " << fixed << setprecision(2) << s.average << "\n";
  cout << "Status: " << s.status << "\n";
  cout << "-----------------------------\n";
}

void showAllStudents() {
  if (students.empty()) {
    cout << "\nNo students found.\n";
    return;
  }

  for (const Student &s : students)
    displayStudent(s);
}

void searchStudent() {
  int id = getInteger("Enter student ID: ");
  int index = findStudent(id);

  if (index == -1)
    cout << "Student not found.\n";
  else
    displayStudent(students[index]);
}

void updateStudent() {
  int id = getInteger("Enter student ID: ");
  int index = findStudent(id);

  if (index == -1) {
    cout << "Student not found.\n";
    return;
  }

  double oldGrades[3];
  for (int i = 0; i < 3; i++) {
    oldGrades[i] = students[index].grades[i];
  }

  for (int i = 0; i < 3; i++) {
    students[index].grades[i] =
        getGrade("Enter new grade for subject " + to_string(i + 1) + ": ");
  }

  calculate(students[index]);

  if (saveData()) {
    cout << "Student updated successfully!\n";
  } else {
    for (int i = 0; i < 3; i++) {
      students[index].grades[i] = oldGrades[i];
    }
    calculate(students[index]);
    cout << "Failed to save changes. Update cancelled.\n";
  }
}

void deleteStudent() {
  int id = getInteger("Enter student ID: ");
  int index = findStudent(id);

  if (index == -1) {
    cout << "Student not found.\n";
    return;
  }

  Student deletedStudent = students[index];
  students.erase(students.begin() + index);

  if (saveData()) {
    cout << "Student deleted successfully.\n";
  } else {
    students.insert(students.begin() + index, deletedStudent);
    cout << "Failed to save changes. Deletion cancelled.\n";
  }
}

void sortStudents() {
  vector<Student> oldStudents = students;

  sort(
      students.begin(), students.end(),
      [](const Student &a, const Student &b) { return a.average > b.average; });

  if (saveData()) {
    cout << "Students sorted by average (highest first).\n";
    showAllStudents();
  } else {
    students = oldStudents;
    cout << "Failed to save changes. Sorting cancelled.\n";
  }
}

void showStatistics() {
  if (students.empty()) {
    cout << "No students available.\n";
    return;
  }

  int passed = 0, failed = 0;
  double totalAverage = 0;
  int highestIndex = 0, lowestIndex = 0;

  for (int i = 0; i < (int)students.size(); i++) {
    totalAverage += students[i].average;

    if (students[i].status == "Passed")
      passed++;
    else
      failed++;

    if (students[i].average > students[highestIndex].average)
      highestIndex = i;

    if (students[i].average < students[lowestIndex].average)
      lowestIndex = i;
  }

  cout << "\n========== CLASS STATISTICS ==========\n";
  cout << "Total students: " << students.size() << "\n";
  cout << "Passed: " << passed << "\n";
  cout << "Failed: " << failed << "\n";
  cout << "Class average: " << fixed << setprecision(2)
       << totalAverage / students.size() << "\n";
  cout << "Highest average: " << students[highestIndex].average << " ("
       << students[highestIndex].name << ")\n";
  cout << "Lowest average: " << students[lowestIndex].average << " ("
       << students[lowestIndex].name << ")\n";
  cout << "======================================\n";
}

void showMenu() {
  cout << "\n====== STUDENT GRADE MANAGER ======\n";
  cout << "1. Add Student\n";
  cout << "2. Show All Students\n";
  cout << "3. Search Student\n";
  cout << "4. Update Student\n";
  cout << "5. Delete Student\n";
  cout << "6. Sort Students by Average\n";
  cout << "7. Class Statistics\n";
  cout << "0. Exit\n";
  cout << "==================================\n";
}
int main() {
  ifstream checkFile(FILE_NAME);
  bool fileExists = checkFile.good();
  checkFile.close();

  if (loadData()) {
    cout << "Student data loaded successfully.\n";
  } else if (fileExists) {
    cout << "Error: Student data file is invalid. Original file preserved.\n";
    return 1;
  } else {
    cout << "No saved data found. Starting with an empty list.\n";
  }
  int choice;
  do {
    showMenu();
    choice = getInteger("Enter your choice: ");

    switch (choice) {
    case 1:
      addStudent();
      break;
    case 2:
      showAllStudents();
      break;
    case 3:
      searchStudent();
      break;
    case 4:
      updateStudent();
      break;
    case 5:
      deleteStudent();
      break;
    case 6:
      sortStudents();
      break;
    case 7:
      showStatistics();
      break;

    case 0:
      if (saveData()) {
        cout << "Data saved successfully.\n";
      } else {
        cout << "Failed to save student data.\n";
      }

      cout << "Thank you for using Student Grade Manager!\n";
      break;

    default:
      cout << "Invalid choice. Try again.\n";
    }

  } while (choice != 0);

  return 0;
}
