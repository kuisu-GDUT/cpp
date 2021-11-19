#include <Python.h>
#include <iostream>

using namespace std;

int main()
{
    Py_Initialize();
    if(!Py_IsInitialized()){
        cout<<"python init fail"<<endl;
        return 0;
    }
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./script')");
    
    PyObject* moduleName = PyUnicode_FromString("Py2Cpp");
    PyObject* pModule = PyImport_Import(moduleName);
    if(pModule == NULL){
        cout <<"module not found"<<endl;
        return 0;
    }

    PyObject* pFunc = PyObject_GetAttrString(pModule, "add_num");
    if(!pFunc){
        cout<<"not found function"<<endl;
        return 0;
    }
    
    PyObject* args = Py_BuildValue("(ii)",28,103);
    PyObject* pRet = PyObject_CallObject(pFunc, args);
    Py_DECREF(args);
    
    int res = 0;
    PyArg_Parse(pRet,"i",&res);//
    Py_DECREF(pRet);
    cout<<res<<endl;
    
    Py_Finalize();
    return 0;
}