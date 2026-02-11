#define PY_SSIZE_T_CLEAN
#include <cstdint>
#include <Python.h>
#include <iostream>
#include <numpy/arrayobject.h>
#include <numpy/ndarraytypes.h>
using namespace std;


int test(int val){
  return val*2;
}


static PyObject* pyTest(PyObject *self, PyObject *args){
  PyArrayObject* ogarr;
  PyArrayObject* testarr;
  cout << "test_a" << endl;
  //cout << type(ogarr) << endl; 
  //
  if (!PyArg_ParseTuple(args, "O!|OO", &PyArray_Type, &testarr,&ogarr)) {
           return NULL;
  }
  cout << "b" << endl;
  // const double* test= static_cast<const double*>(PyArray_DATA(ogarr));
  // for (auto i=0;i!=3;++i){
  //      cout << test[i] << endl;
  // };
  PyArray_Descr* reqDescr= PyArray_DescrFromType(NPY_DOUBLE);
  PyArrayObject* arr= (PyArrayObject*)PyArray_FromArray(ogarr,reqDescr,NPY_ARRAY_CARRAY);
  double* dataPtr = static_cast<double*>(PyArray_DATA(arr));
  for (int i=0; i<7;i++){

    cout << dataPtr[i] << endl;
    
  }
  PyArrayObject* outarr;

  PyObject* t= PyArray_Sum(testarr,0,NPY_FLOAT32,outarr);

  return Py_BuildValue("O",t);
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
