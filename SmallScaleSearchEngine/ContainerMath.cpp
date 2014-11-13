//
//  ContainerMath.cpp
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/11/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#include "ContainerMath.h"

double sum(const std::vector<double> & v) {
    double ans = 0;
    for (int i=0; i<v.size(); ++i) ans += v[i];
    return ans;
}

double abs(const double & a) {
    return a > 0 ? a : -a;
}

double absSum(const std::vector<double> & a) {
    double sum;
    for (int i=0; i<a.size(); ++i) sum += abs(a[i]);
    return sum;
}

std::vector<double> & operator-(std::vector<double> & a, double b) {
    for (int i=0; i<a.size(); ++i) a[i] -= b;
    return a;
}

std::vector<double> & operator+(std::vector<double> & a, double b) {
    for (int i=0; i<a.size(); ++i) a[i] += b;
    return a;
}

std::vector<double> & operator-(std::vector<double> & a, const std::vector<double> & b) {
    for (int i=0; i<a.size(); ++i) a[i] -= b[i];
    return a;
}

std::vector<double> & operator+(std::vector<double> & a, const std::vector<double> & b) {
    for (int i=0; i<a.size(); ++i) a[i] += b[i];
    return a;
}