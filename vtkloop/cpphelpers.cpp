#define PY_SSIZE_T_CLEAN
#include <cstdint>
#include <Python.h>
#include <iostream>
#include <stdint.h>             
#include <vector>
#include <unordered_map>       
#include <sstream>
#include <numpy/arrayobject.h>
#include <numpy/ndarraytypes.h>
using namespace std;


int test(int val){
  return val*2;
}
static int mod(int a, int b){
  return ((a%b)+b)%b;
}

static int floordiv(int a, int b) {
    int q = a / b;
    int r = a % b;
    if ((r != 0) && ((r > 0) != (b > 0))) {
        q--;
    }
    return q;
}

static vector<int64_t> children(int cid, int level,vector<int64_t>& cid_offsets,vector<uint64_t>& xcells,vector<uint64_t>& ycells,vector<uint64_t>& zcells,vector<int64_t>& out){
  vector<vector<int32_t>> delta={{0,0,0},{1,0,0},{0,1,0},{1,1,0},{0,0,1},{1,0,1},{0,1,1},{1,1,1}};
  long cellid=cid-1-cid_offsets[level];
  vector<int32_t> cellind(3,-1);
  cellind[0]=mod(cellid,(xcells[level]))*2;
  cellind[1]=mod(floordiv(cellid,xcells[level]),(ycells[level]))*2;
  cellind[2]=floordiv(cellid,xcells[level]*ycells[level])*2;
  //vector<int64_t> out(8,0);
  for (size_t i=0;i < out.size(); i++){
    out[i]=cid_offsets[level+1] + (cellind[0] + delta[i][0]) + xcells[level+1]*(cellind[1]+ delta[i][1])+
       (cellind[2] + delta[i][2])*xcells[level+1]*ycells[level+1] + 1;
  }
  return out;
}

static int convertToUnordMap(PyObject* dict, unordered_map<int,uint64_t>& map){
  PyObject *key, *value;
  
  Py_ssize_t pos=0;

  while (PyDict_Next(dict,&pos,&key,&value)){
    
    uint64_t val = PyLong_AsUnsignedLongLong(value);
    long keyval = PyLong_AsLong(key);
    map[keyval]=val;
    if (PyErr_Occurred()){
      return 1;
    }

  }
  return 0;
}

static PyObject* pyTest(PyObject *self, PyObject *args){
  PyArrayObject* ogarr;
  int max_ref_level;
  PyObject* fileindex_for_cellid;
  int xc,yc,zc;
  stringstream descr;
  //O!|OO (1 required arg (PythonObject) with 2 optional (not sure why we need ! on the first one))
  // more args O!|O|i for integer
  if (!PyArg_ParseTuple(args, "O!|O|i|i|i|i", &PyArray_Type, &ogarr, &fileindex_for_cellid,&xc,&yc,&zc,&max_ref_level)) {
           return NULL;
  }
  
  
  // PyArray_Descr* reqDescr= PyArray_DescrFromType(NPY_DOUBLE);
  // PyArrayObject* arr= (PyArrayObject*)PyArray_FromArray(ogarr,reqDescr,NPY_ARRAY_CARRAY);
  // double* dataPtr = static_cast<double*>(PyArray_DATA(arr));
  //cout << PyDict_Contains(fileindex_for_cellid,Py_BuildValue("i",34965)) << endl;

  //unordered_map<int,PyObject*> idxToFileIndex;
  vector<uint64_t> xcells(max_ref_level+1,0);
  vector<uint64_t> ycells(max_ref_level+1,0);
  vector<uint64_t> zcells(max_ref_level+1,0);
  
  for (int r=0;r<max_ref_level+1;r++){
    xcells[r]=xc*pow(2,r);
    ycells[r]=yc*pow(2,r);
    zcells[r]=zc*pow(2,r);
  }
  unordered_map<int,uint64_t> idxToFileIndex;
  unordered_map<int,uint64_t> fileindex_for_cellid_map;
  if ( convertToUnordMap(fileindex_for_cellid, fileindex_for_cellid_map) != 0){
    cout << "error" << endl;
  }
  int idx=0;
  vector<vector<int>> subdivided(max_ref_level+1);
  vector<int64_t> cid_offsets(max_ref_level+1,0);
  uint64_t isum = 0;
  for (size_t p=0;p<max_ref_level;p++){
    isum= isum+ pow(2,3*p)*xc*yc*zc;
    cid_offsets[p+1] = isum;
  }
  //vector<int64_t> childs= children(34965,2,cid_offsets,xcells,ycells,zcells);

  for (int c=1;c<xc*yc*zc+1;c++){
    if (fileindex_for_cellid_map.find(c) != fileindex_for_cellid_map.end()){
      //Write
      descr.put('.');
      idxToFileIndex[idx]=fileindex_for_cellid_map[c];
    }
    else{
      descr.put('R');
      subdivided[0].push_back(c);
    }
    idx=idx+1;
  }
  vector<int64_t> childs;
  childs.reserve(8); 
  for (int l=1;l<max_ref_level+1;l++){
    auto& vecptr=subdivided[l-1];
    auto& subd=subdivided[l];
    for (int it : vecptr){
     //vector<int64_t> childs=children(*it,l-1,cid_offsets,xcells,ycells,zcells);
     childs.clear(); 
     children(it,l-1,cid_offsets,xcells,ycells,zcells,childs);
     for (int64_t child : childs){
        auto it2 = fileindex_for_cellid_map.find(child); 
        if (it2!= fileindex_for_cellid_map.end()){
          descr.put('.');
          idxToFileIndex[idx] = it2->second;
        }
        else{
          descr.put('R');
          subd.push_back(child);
        }
        idx=idx+1;
      }
    }
    if (l<max_ref_level){
      descr.put('|');
    }
  } 
  // for (int i=0; i<7;i++){
  //
  //   cout << dataPtr[i] << endl;
  //
  // }
  //PyArrayObject* outarr;

  //PyObject* t= PyArray_Sum(ogarr,0,NPY_FLOAT32,outarr);

  return Py_BuildValue("i",0);
}

static PyMethodDef cpphelpers_methods[] = {
  {"test",pyTest,METH_VARARGS,"test"},
  {NULL}        /* Sentinel */
};
static struct PyModuleDef cpphelpers = {
    .m_methods = cpphelpers_methods,
};
PyMODINIT_FUNC PyInit_cpphelpers(void)
// create the module
{
  import_array();
  return PyModuleDef_Init(&cpphelpers);
}
