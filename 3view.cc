#include <iostream>
#include <fstream>
#include <cstring>

using std::cout;
using std::endl;
using std::cerr;
using std::cin;
using std::ifstream;
using std::istream;
using std::ios;

void Usage(char * argv[]);
const char * header [] = {
  "Display  \"Object Test\"  \"Screen\" \"rgbsingle\"",
  "",
  "ObjectBegin \"Object\""
};

const char * tailer [] = {
"ObjectEnd",
"",
"CameraEye 0 0 100",
"CameraAt  0 0 0",
"CameraUp  0 1 0",
"CameraFOV 20",
"",
"Background 0.5 0.5 0.5",
"",
"WorldBegin",
"",
"FarLight 0 0 -1 1 1 1 0.8",
"FarLight -1 0 0 1 0 0 0.6",
"FarLight 0 -1 0 0 1 0 0.6",
"FarLight 1 0 0 0 1 1 0.6",
"FarLight 0 1 0 1 0 1 0.6",
"Ka 0.3",
"Kd 0.8",
"",
"XformPush",
"Translate -10 10 0",
"Rotate \"Z\" -90",
"Rotate \"Y\" -90",
"ObjectInstance \"Object\"",
"XformPop",
"",
"XformPush",
"Translate -10 -10 0",
"Rotate \"Z\" 180"
"Rotate \"X\" 90",
"ObjectInstance \"Object\"",
"XformPop",
"",
"XformPush",
"Translate 10 -10 0",
"ObjectInstance \"Object\"",
"XformPop",
"",
"XformPush",
"Translate 10 10 0",
"Rotate \"X\" 30",
"Rotate \"Y\" -30",
"Rotate \"Z\" -90",
"Rotate \"Y\" -90",
"ObjectInstance \"Object\"",
"XformPop",
"",
"WorldEnd"

};

int main(int argc, char * argv[])
{
  if(argc < 2)
    {
      Usage(argv);
      return 0;
    }

  istream * inp = nullptr;
  ifstream inp_file;

  // Connect to correct stream
  if(!strcmp(argv[1], "-"))
    inp = & cin;
  else
    {
      inp_file.open(argv[1], ios::binary);
      
      if(!inp_file)
	{
	  cerr << "Can't open " << argv[1] << endl;
	  return -1;
	}

      inp = & inp_file;
    }

  // Print out header

  int n = sizeof(header) / sizeof (char *);

  for(int i = 0; i < n; i++)
    {
      cout << header[i] << endl;
    }


  // Insert object
  char transfer;

  transfer = inp->get();

  while(!inp->eof())
    {
      cout.put(transfer);
      transfer = inp->get();
    }

  if(inp != & cin)
    inp_file.close();
  
  // Output tailer
  n = sizeof(tailer) / sizeof(char *);

  for(int i = 0; i < n; i++)
    cout << tailer[i] << endl;
}

void Usage(char * argv[])
{
  cerr << "Usage: " << argv[0] << "   polyset_file\n";
  cerr << "   results are sent to the standard output and can be\n";
  cerr << "   redirected to a file. Using the polyset_file name of '-'\n";
  cerr << "   takes input from the standard input.\n";
}