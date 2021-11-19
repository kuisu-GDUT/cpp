#!/usr/bin/env python
# -*- coding: UTF-8 -*-
'''
@Project ：pyProject 
@File    ：linear_fit.py
@Author  ：kuisu
@Email     ：kuisu_dgut@163.com
@Date    ：2021/11/15 14:14 
'''
import numpy as np
from numpy.linalg import inv  # 矩阵求逆
from numpy import dot  # 矩阵点乘
from numpy import mat  # 二维矩阵
import bisect
import pandas as pd
import os
import openpyxl

#0. 计算权重
class GroupLinearFit(object):
    def __init__(self,x,y,subsection=1) -> None:
        super().__init__()
        self.x = x
        self.y = y
        self.subsection = subsection


    def weights(self,X, Y):
        assert X.shape == Y.shape
        if isinstance(X, np.ndarray):
            X = mat(X).reshape(-1,1)
            Y = mat(Y).reshape(-1,1)
        result = dot(dot(inv(dot(X.T, X)), X.T), Y)
        return result

    # 1. 排序
    def sort(self):
        x,y = self.x, self.y
        assert len(x)==len(y)
        idx = np.argsort(x)
        sort_x = x[idx]
        sort_y = y[idx]
        return sort_x,sort_y

    #2. 分段
    def group_weights(self):
        sort_x, sort_y = self.sort()
        subsection = self.subsection
        groups = int(np.ceil(len(sort_x)/subsection))
        weight_groups = []
        for i in range(groups):
            start = i*subsection
            end = min((i+1)*subsection,len(sort_x))
            sub_x = sort_x[start:end]
            sub_y = sort_y[start:end]
            weight = self.weights(sub_x, sub_y)
            weight_groups.append(weight)
        return weight_groups,sort_x,self.subsection

def name(name):
    print('you name is {}'.format(name))

def InitLinear(path):
    '''
    初始化线性分段函数
    :param path:
    :return: 返回初始化的GroupLinearFit类
    '''
    if not os.path.exists(path):
        print("{} no exits".format(path))
        return None
    else:
        print("loading template file from {}".format(path))
    data = pd.read_excel(io=path)
    x = np.array(data.x)
    y = np.array(data.y)
    subsection = 2
    Linear = GroupLinearFit(x, y, subsection)

    return Linear

def calculateValue(Linear,input_value=1):
    #assert isinstance(input_value),'please significant number'
    assert isinstance(Linear,GroupLinearFit),"please input correct Linear"
    print(os.getcwd())
    #1. 求每组的权重
    weight_groups,sort_x,group_num = Linear.group_weights()#权重, 分组数
    #2. 获取当前输入的值所在的组别
    input_new_idx = bisect.bisect_right(sort_x,input_value)
    group = input_new_idx//group_num
    # print(group,';',weight_groups[group])
    #3. 计算当前输入的所对应的输出值
    result_matrix = weight_groups[group]*input_value#convert matrix to array
    result = result_matrix.A[0][0]#shape is [1,1]
    print("input:{:.3g}, result:{:.3g}".format(input_value,result))
    return result

def calculateExcel(Linear,path):
    '''
    计算excel的 output value
    :param Linear: weight
    :param path: calculate Path
    :return: excel
    '''
    input_data = pd.read_excel(path)
    input_x = input_data.x
    input_x = np.array(input_x)
    weight_groups, sort_x, group_num = Linear.group_weights()  # 权重, 分组数
    groups = []
    for x in input_x:
        input_new_idx = bisect.bisect_right(sort_x, x)
        group = input_new_idx // group_num
        groups.append(group)
    weights = [weight_groups[group] for group in groups]
    weights = np.array(weights).squeeze()
    result = weights*input_x
    result_df = pd.DataFrame({"input":input_x,"result":result})
    print("input path of excel:{}\n, result:{}".format(path,result_df))
    result_df.to_excel('result.xlsx')


if __name__ == '__main__':
    Linear = InitLinear(path='./template.xlsx')
    calculateValue(Linear=Linear,input_value=5)
    calculateExcel(Linear,path='./calculate.xlsx')