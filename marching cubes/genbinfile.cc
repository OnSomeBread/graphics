#include <math.h>

#include <fstream>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

int main() {
    string filename = "output.bin";
    std::ofstream file(filename, std::ios::binary);

    int cols = 10, rows = 10, planes = 10;
    int index = 1;
    vector<float> values;

    float mp = planes / 2;
    float mr = rows / 2;
    float mc = cols / 2;

    for (int p = 0; p < planes; ++p) {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                float x = (p - mp) * (p - mp);
                float y = (r - mr) * (r - mr);
                float z = (c - mc) * (c - mc);
                float dist = sqrt(x + y + z);
                values.push_back(static_cast<float>(dist));
            }
        }
    }

    file << cols << " " << rows << " " << planes << " ";
    for (int i = 0; i < (int)values.size(); ++i) {
        file.write(reinterpret_cast<const char*>(&values[i]), sizeof(float));
    }

    file.close();
}