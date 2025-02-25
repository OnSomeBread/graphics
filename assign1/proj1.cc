#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "polyread.cc"

using std::cout;
using std::endl;
using std::stof;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

void skew(vector<vector<float>> &coords, float skewx, float skewy);
void twist(vector<vector<float>> &coords, int degrees);
void extrude(vector<vector<float>> &coords, vector<vector<int>> &facesList,
             int amount);
void sweep(vector<vector<float>> &coords, vector<vector<int>> &facesList,
           int steps, int angle);
void writeOutput(string fileName, vector<vector<float>> &coords,
                 vector<vector<int>> &facesList);

int main(int argc, char *argv[]) {
    if (argc < 5) {
        cout << "Usage ./project1 inputFileName outputFileName technique "
                "techniqueArgs\n";
        cout << "possible techniques and args are\n"
             << "skew skewX skewY\n";
        cout << "twist degrees\n"
             << "extrude extrudeAmount\n";
        cout << "sweep steps degrees\n";
        return 0;
    }

    int verts = 0;
    int faces = 0;
    float Oldcoords[10000];
    int vertList[10000];
    std::ifstream inFile(argv[1]);
    string psType = "P";

    int rt = parse_polyset(inFile, psType, verts, faces, Oldcoords, vertList);

    if (rt != 0) {
        return rt;
    }

    // turn the array of floats into vector<vector<float>> to make working on
    // the techniques easier each sub vector is size 3 for the 3 coords
    vector<vector<float>> coords;
    for (int i = 0; i < verts; i++) {
        int j = i * 3;

        // can't just use arr init here?? weird compiler
        vector<float> n;
        n.push_back(Oldcoords[j]);
        n.push_back(Oldcoords[j + 1]);
        n.push_back(Oldcoords[j + 2]);

        coords.push_back(n);
    }

    // turns int array of faces into a vector where facesList[i] = vector of all
    // of the vector points without the -1
    vector<vector<int>> facesList;
    vector<int> curr;
    int faceCount = 0;
    for (int i = 0; i < 10000; i++) {
        if (vertList[i] == -1) {
            facesList.push_back(curr);
            curr.clear();
            faceCount++;
            if (faceCount >= faces) break;
        } else {
            curr.push_back(vertList[i]);
        }
    }

    if (strcmp(argv[3], "skew") == 0) {
        skew(coords, stof(argv[4]), stof(argv[5]));
    } else if (strcmp(argv[3], "twist") == 0) {
        twist(coords, stoi(argv[4]));
    } else if (strcmp(argv[3], "extrude") == 0) {
        extrude(coords, facesList, stof(argv[4]));
    } else if (strcmp(argv[3], "sweep") == 0) {
        sweep(coords, facesList, stoi(argv[4]), stoi(argv[5]));
    } else {
        cout << "invalid technique" << endl;
        return 0;
    }

    writeOutput(argv[2], coords, facesList);
}

// adds a constant to each x and y coord in the polyset
void skew(vector<vector<float>> &coords, float skewx, float skewy) {
    for (long unsigned int i = 0; i < coords.size(); i++) {
        coords[i][0] += skewx * coords[i][2];
        coords[i][1] += skewy * coords[i][2];
    }
}

// performs a rotation from x to y
void twist(vector<vector<float>> &coords, int degrees) {
    // translate to radians
    float theta = degrees * (M_PI / 180);
    for (long unsigned int i = 0; i < coords.size(); i++) {
        float newx = coords[i][0] * cos(theta * coords[i][2]) -
                     coords[i][1] * sin(theta * coords[i][2]);
        float newy = coords[i][0] * sin(theta * coords[i][2]) +
                     coords[i][1] * cos(theta * coords[i][2]);
        coords[i][0] = newx;
        coords[i][1] = newy;
    }
}

// extrudes the polyset out from the z axis for the amount specified
void extrude(vector<vector<float>> &coords, vector<vector<int>> &facesList,
             int amount) {
    vector<vector<float>> newVerts;
    // create all the new z coords
    for (long unsigned int i = 0; i < coords.size(); i++) {
        vector<float> c;
        c.push_back(coords[i][0]);
        c.push_back(coords[i][1]);
        c.push_back(coords[i][2] + amount);
        newVerts.push_back(c);
    }

    // this is because we are adding to facesList but don't want bounds to go
    // further
    int s = facesList.size();
    for (int i = 0; i < s; i++) {
        int numVerts = facesList[i].size();
        for (int j = 0; j < numVerts; j++) {
            int v1 = facesList[i][j];
            int v2 = facesList[i][(j + 1) % numVerts];
            int v3 = v1 + coords.size();
            int v4 = v2 + coords.size();

            vector<int> newc;
            newc.push_back(v2);
            newc.push_back(v4);
            newc.push_back(v3);
            newc.push_back(v1);

            facesList.push_back(newc);
        }
    }

    // cap the end
    for (int i = 0; i < s; i++) {
        int numVerts = facesList[i].size();
        vector<int> face;
        for (int j = 0; j < numVerts; j++) {
            face.push_back(facesList[i][j] + coords.size());
        }
        facesList.push_back(face);
    }

    // adds all of the new coords to the main vector
    coords.insert(coords.end(), newVerts.begin(), newVerts.end());
}

// performs a sweep on the polyset
void sweep(vector<vector<float>> &coords, vector<vector<int>> &facesList,
           int steps, int angle) {
    vector<vector<float>> newcoords;
    vector<vector<int>> newfacesList;

    float stepSize = angle * (M_PI / 180) / steps;
    int base = 0;
    int verts = coords.size();
    for (int step = 0; step <= steps; step++) {
        float theta = stepSize * step;

        // create all the new coords
        for (int i = 0; i < verts; i++) {
            // technically here all z coords are meant to be zero so the right
            // side would cancel but ill leave it here for future reference
            vector<float> c;
            c.push_back(coords[i][0] * cos(theta) + coords[i][2] * sin(theta));
            c.push_back(coords[i][1]);
            c.push_back(-coords[i][0] * sin(theta) + coords[i][2] * cos(theta));
            newcoords.push_back(c);
        }

        if (step > 0) {
            // this is because we are adding to facesList but don't want bounds
            // to go further
            int s = facesList.size();
            for (int i = 0; i < s; i++) {
                int numVerts = facesList[i].size();
                for (int j = 0; j < numVerts; j++) {
                    int v1 = facesList[i][j] + base;
                    int v2 = facesList[i][(j + 1) % numVerts] + base;
                    int v3 = v1 + coords.size();
                    int v4 = v2 + coords.size();

                    vector<int> newc;
                    newc.push_back(v1);
                    newc.push_back(v3);
                    newc.push_back(v4);
                    newc.push_back(v2);

                    newfacesList.push_back(newc);
                }
            }
        }

        base += verts;
    }

    // cap the end of the sweep
    int s = facesList.size();
    for (int i = 0; i < s; i++) {
        vector<int> face;
        int numVerts = facesList[i].size();
        for (int j = numVerts - 1; j > 0; j--) {
            face.push_back(facesList[i][j] + base);
        }
        facesList.push_back(face);
    }

    // adds all of the new coords to the main vector
    coords.insert(coords.end(), newcoords.begin(), newcoords.end());
    facesList.insert(facesList.end(), newfacesList.begin(), newfacesList.end());
}

// turns lines into lines of a new file
void writeOutput(string fileName, vector<vector<float>> &coords,
                 vector<vector<int>> &facesList) {
    // write all changes to a new file starting with the header
    string output = "PolySet \"P\" " + to_string(coords.size()) + " " +
                    to_string(facesList.size()) + "\n";
    for (long unsigned int i = 0; i < coords.size(); i++) {
        output += to_string(coords[i][0]) + " " + to_string(coords[i][1]) +
                  " " + to_string(coords[i][2]) + "\n";
    }

    // now add all of the faces to the output file
    output += "\n";
    for (long unsigned int i = 0; i < facesList.size(); ++i) {
        for (long unsigned int j = 0; j < facesList[i].size(); ++j) {
            output += to_string(facesList[i][j]) + " ";
        }
        output += "-1\n";
    }

    // write to output file
    std::ofstream outputFile(fileName);
    outputFile << output;
    outputFile.close();
}