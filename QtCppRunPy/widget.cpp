#include <Python.h>
#include "widget.h"
#include "ui_widget.h"
#include <iostream>
#include <QDebug>



Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    using namespace std;
    qDebug()<<"clicked ...";
    runPy();
}

int Widget::runPy()
{
    Py_Initialize();
    if (!Py_IsInitialized())
    {
        qDebug()<<"python init fall";

    }

    PyObject* pModule = PyImport_ImportModule("linearFit");
    if (pModule==NULL)
    {
        qDebug()<<"module not found";
        return 1;
    }

    PyObject* pFunc = PyObject_GetAttrString(pModule,"calculate");
    if (!pFunc)
    {
        qDebug()<<"not found function add_num";
        return 0;
    }
    PyObject_CallObject(pFunc,NULL);

    Py_Finalize();
    return 0;
}
