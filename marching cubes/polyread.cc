#include <iostream>
#include <string>

using std::istream;
using std::string;


/****************************************************************************/
// Function declarations

static void EatSpace(istream& );
// Removes extraneous spaces and comments from input

int  get_vertex_size(const string & vertex_type);
  // Given a vertex type string returns the number of array elements
  // taken up by a single vertex.


int parse_polyset(istream& istr, string & vtype, 
		  int & nvertex, int & nface, 
		  float coord [], int vertex_list[]);

  // Given an input stream returns 
  //     the vertex type string (vtype), 
  //     the number of vertices (nvertex),
  //     the number of faces (nface),
  //     and the values of the vertex coordinate (coord)
  //     and the values of the polygon vertex indices (vertex_list)
  // The arrays must already have memory allocated to them.

int  set_attribute_indices(const string & vertex_type, 
			   int & vertex_size,
			   int & geometry,
			   int & color,
			   int & normal,
			   int & texture);

  // Given a vertex type string returns
  //    the number of elements taken by a vertex (vertex_size),
  //    the index positions of the start of various attributes within a vertex.
  //    Current attributes include geometry, color, normal, and texture.
  //    The sizes of each attribute are as follows:
  //    geometry - 3 values
  //    color - 3 values
  //    normal - 3 values
  //    texture - 2 values

/****************************************************************************/
// Global variables

int rd_input_line = 0;
// Useful for error messages
int OK = 0;
int ILLEGAL_TOKEN = 10;
int UNEXPECTED_EOF = 20;
int ILLEGAL_VERTEX_INDEX = 30;
int VERTEX_LIST_MAX_SIZE = 10000;
int VERTEX_LIST_OVERFLOW = 40;
int UNKNOWN_VERTEX_TYPE = 50;


/****************************************************************************/

static void	EatSpace(istream& file){
  // Eat white space and comments
  
  int dummychar;
  int valid = 0;
  
  // eat whitespace and comments
  
  do{
    dummychar = file.get();
    if(file.eof()) return;               // Read past end of file
    switch(dummychar){
    case ' ':                            // Eat white space
    case '\t':
    case '\r':   // Needed for DOS files
      break;
    case '\n':
      rd_input_line++;
      break;
    case '#':                            // Eat comments
      while('\n' != file.get() && !file.eof());
      if(file.eof())  return;
      rd_input_line++;
      break;
    default:
      file.putback(dummychar);
      valid = 1;
      break;
    }
  }while(!valid);
}

int parse_polyset(istream& istr, string & vtype, 
			 int & nvertex, int & nface, 
			 float coord [], int vertex_list[])
{
  string token;

  int vsize;
  int size;

  int err = OK;

  EatSpace(istr);

  istr >> token;

  if(token != "PolySet")
    return ILLEGAL_TOKEN;

  EatSpace(istr);

  if(istr.eof())
    return UNEXPECTED_EOF;

  istr >> vtype;
  vsize = get_vertex_size(vtype);
  if(vsize < 0) // error code
    return vsize;

  EatSpace(istr);
  istr >> nvertex;
  EatSpace(istr);
  istr >> nface;

  size = vsize * nvertex;

  for(int i = 0; i < size; i++){
      EatSpace(istr);
      if(istr.eof()){
	  return UNEXPECTED_EOF;
	}
      double dvalue;

      EatSpace(istr);
      istr >> dvalue;

      coord[i] = dvalue;
    }

  int index_cnt = 0;
  int index;

  for(int i = 0; i < nface;){
      EatSpace(istr);
      if(istr.eof()){
	  return UNEXPECTED_EOF;
	}

      EatSpace(istr);
      istr >> index;

      if(index >= nvertex){
	  return ILLEGAL_VERTEX_INDEX;
	}
      vertex_list[index_cnt++] = index;
      if(index_cnt > VERTEX_LIST_MAX_SIZE){
	  return VERTEX_LIST_OVERFLOW;
	}
      if(index == -1) i++;  // New face
    }

  
  return err;
}


int  get_vertex_size(const string & vertex_type){
  const char * attribute = &vertex_type[0];
  int i = vertex_type.length();
  int vertex_size = 0;
  
  for(; i; attribute++, i--){
      switch(*attribute){
        case '\"': // Ignore \" 
          break;
        case 'P':
          vertex_size += 3;
          break;
        case 'N': case 'D':
          vertex_size += 3;
          break;
        case 'C':
          vertex_size += 3;
          break;
        case 'T':
          vertex_size += 2;
          break;
        default:
          break;
        }
    }

  return vertex_size;
}


int  set_attribute_indices(const string & vertex_type, 
                                  int & vertex_size,
                                  int & geometry,
                                  int & color,
                                  int & normal,
                                  int & texture){
  int err = OK;
  const char * attribute = &vertex_type[0];
  int i = vertex_type.length();
  vertex_size = 0;
  
  geometry = color = normal = texture = -1;  // Not used

  for(; i; attribute++, i--){
      switch(*attribute){
        case '\"': // Ignore \" 
          break;
        case 'P':
          geometry = vertex_size;
          vertex_size += 3;
          break;
        case 'N': case 'D':
          normal = vertex_size;
          vertex_size += 3;
          break;
        case 'C':
          color = vertex_size;
          vertex_size += 3;
          break;
        case 'T':
          texture = vertex_size;
          vertex_size += 2;
          break;
        default:
          err = UNKNOWN_VERTEX_TYPE;
          break;
        }
    }

  return err;
}