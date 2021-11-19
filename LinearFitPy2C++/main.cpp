#include <Python.h>
#include <iostream>
#include <string>

using namespace std;

int main()
{
    Py_Initialize();
    if(!Py_IsInitialized()){
        cout<<"python init fail"<<endl;
        return 0;
    }
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./script')");//append file of script
    
    PyObject* moduleName = PyUnicode_FromString("linear_fit");
    PyObject* pModule = PyImport_Import(moduleName);
    if(pModule == NULL){
        cout <<"module not found"<<endl;
        return 0;
    }

    //define Linear class
    PyObject *pModuleInitLinear = PyObject_GetAttrString(pModule,"InitLinear");
    if(!pModuleInitLinear){
        cout<<"not found function"<<endl;
        return 0;
    }


    // const string adf = "script\\template.xlsx";
    PyObject* argsPath = Py_BuildValue("(s)","script\\template.xlsx");//path of template, and it is PyObject
    PyObject* pLinear = PyObject_CallObject(pModuleInitLinear,argsPath);//wait to amend

    PyObject* argsLinear = Py_BuildValue("O",pLinear);//python object

    PyObject* pFunc = PyObject_GetAttrString(pModule, "calculateValue");
    if(!pFunc){
        cout<<"not found function"<<endl;
        return 0;
    }

 
    //input values of excel to calculate
    PyObject* argsExcelCalculate = Py_BuildValue("(Os)",argsLinear,"script\\calculate.xlsx");
    PyObject* pExcelFunc = PyObject_GetAttrString(pModule, "calculateExcel");
    if(!pExcelFunc){
        cout<<"not found function of pExcelFunc"<<endl;
        return 0;
    }
    PyObject* pExcelRet = PyObject_CallObject(pExcelFunc, argsExcelCalculate);


    //input value to calculate
    while (true)
    {
        float input_value = 2.1;//must define dataType of the variable
        cout <<"please input value: ";
        cin >> input_value;

        PyObject* args = Py_BuildValue("(Of)",argsLinear,input_value);//Python tuple, it clude Object of python and float of python
        PyObject* pRet = PyObject_CallObject(pFunc, args);
        Py_DECREF(args);
        
        float res = 1.2;
        PyArg_Parse(pRet,"f",&res);//PyObject of Python to int of C
        Py_DECREF(pRet);
        cout<<res<<endl;
    }
    
    
    
    Py_Finalize();
    return 0;
}