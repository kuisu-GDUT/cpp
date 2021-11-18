#include <Python.h>

int main(int argc, char* argv[])
{

    //initialize python
    Py_Initialize();
    
    //run source code of python£¬invoking time library of python to get current time
    PyRun_SimpleString("from time import time,ctime\n" "print('Today is', ctime(time()))\n");
    
    //release python
    Py_FinalizeEx(); 
    
    return 0;
}
