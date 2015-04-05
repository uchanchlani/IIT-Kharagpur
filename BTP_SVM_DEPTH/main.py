"""
Author: Utkarsh Chanchlani
Dated: 5th April, 2015
Latest Edit: 5th April, 2015
Version: 1.0

A python script to train and test the depth data and find its utility
in future applications.
"""
#!/usr/bin/python

import PyML
from PyML.classifiers import multi

def getData(fileName):
    fp = open(fileName, "r");
    retlist = [];
    retdata = [];
    retclass = [];
    raw_data = fp.read().splitlines();
    for data_line in raw_data:
        if (len(data_line) == 0 or data_line[0] == '\n' or data_line[0] == '\0' or data_line[0] == ' '): 
            continue;
        data = data_line.split(' ');
        retclass.append(data[0]);
        templist = [];
        data.remove(data[0]);
        for attr in data:
            if (len(attr) > 0 and attr[0] != '\n' and attr[0] != '\0' and attr[0] != ' '):
                    templist.append(float(attr));
        retdata.append(templist);
    fp.close();
    retlist.append(retclass);
    retlist.append(retdata);
    return retlist;

if __name__ == '__main__' : 
    print __doc__;
    trainSet = getData("train.dat");
    trainClass = trainSet[0];
    trainData = trainSet[1];
    #print trainData;
    #print trainClass;
    trainVec = PyML.VectorDataSet(trainData,L=trainClass);
    mc = multi.OneAgainstRest(PyML.SVM());
    mc.train(trainVec);
    mc.save("trainedsvm.dat");
    testSet = getData("test.dat");
    testClass = testSet[0];
    testData = testSet[1];
    #print testData;
    #print testClass;
    testVec = PyML.VectorDataSet(testData,L=testClass);
    result = mc.test(testVec);
    print result;
