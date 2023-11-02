#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <thread>

namespace py = pybind11;
using namespace std;

char infer_type(const string& value) {
    if (value.find_first_not_of("0123456789") == string::npos) return 'i';  // is an int
    if (value.find('.') != string::npos) return 'f';  // is a float
    if (value == "true" || value == "false") return 'b';  // is a bool
    return 's';  // String
}

py::object convert(const string& value, const char type) {
    try {
        if (type == 'i') return py::cast(stoi(value));
        if (type == 'f') return py::cast(stof(value));
        if (type == 'b') return py::cast(value == "true");
    } catch (std::invalid_argument& e) {
        throw py::value_error("Conversion error: " + string(e.what()) + " Increase type_inference_lookahead or set type_inference to False.");
    }
    return py::cast(value);
}

void infer_types(vector<char>& column_types, const string& file_path, const int& lookahead) {
    ifstream file(file_path);
    string line, cell;

    // get the headers
    getline(file, line);
    stringstream header(line);
 
    // load the data
    int row_count = 0;
    while (getline(file, line) && row_count < lookahead) {
        stringstream ss(line);
        int index = 0;
        while (getline(ss, cell, ',')) {
            char& col_type = column_types[index];
            if (col_type != 's') col_type = infer_type(cell);
            index++;
        }
        row_count++;
    }
}

vector<map<string, py::object>> load(const string& file_path, const int& type_inference_lookahead=100, const bool& type_inference=true) {
    vector<map<string, py::object>> result;
    ifstream file(file_path);
    string line, cell;

    // get the headers
    getline(file, line);
    stringstream header(line);
    vector<string> columns;
    
    while (getline(header, cell, ',')) {
        columns.push_back(cell);
    }

    vector<char> column_types(columns.size(), 'u');

    if (type_inference) {
        infer_types(column_types, file_path, type_inference_lookahead);
    }

    // load the data
    while (getline(file, line)) {
        stringstream ss(line);
        map<string, py::object> row;
        int index = 0;
        while (getline(ss, cell, ',')) {
            char& col_type = column_types[index];
            row[columns[index]] = convert(cell, col_type);
            index++;
        }
        result.push_back(row);
    }
    return result;
}

PYBIND11_MODULE(csvtodict, m) {
    m.doc() = "pybind11 csv-to-dict plugin";
    m.def("load", &load, 
          py::arg("file_path"), 
          py::arg("type_inference_lookahead")=100, 
          py::arg("type_inference")=true);
}
