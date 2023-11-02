#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

namespace py = pybind11;
using namespace std;

char infer_type(const string& value) {
    if (value.find_first_not_of("0123456789") == string::npos) return 'i';  // is an int
    if (value.find('.') != string::npos) return 'f';  // is a float
    return 's';  // String
}

py::object convert(const string& value, char type) {
    try {
        if (type == 'i') return py::cast(stoi(value));
        if (type == 'f') return py::cast(stof(value));
    } catch (std::invalid_argument& e) {
        // Do nothing
    }
    return py::cast(value);
}

vector<map<string, py::object>> load(const string& file_path) {
    vector<map<string, py::object>> result;
    ifstream file(file_path);
    string line, cell;

    // get the headers
    getline(file, line);
    stringstream header(line);
    vector<string> columns;
    vector<char> column_types;
    while (getline(header, cell, ',')) {
        columns.push_back(cell);
        column_types.push_back('u'); // u for unknown
    }

    // load the data
    while (getline(file, line)) {
        stringstream ss(line);
        map<string, py::object> row;
        int index = 0;
        while (getline(ss, cell, ',')) {
            char& col_type = column_types[index];
            if (col_type == 'u') col_type = infer_type(cell);
            row[columns[index]] = convert(cell, col_type);
            index++;
        }
        result.push_back(row);
    }
    return result;
}

PYBIND11_MODULE(csvtodict, m) {
    m.doc() = "pybind11 csv-to-dict plugin";
    m.def("load", &load, "A function that reads a CSV file and returns a list of dictionaries");
}
