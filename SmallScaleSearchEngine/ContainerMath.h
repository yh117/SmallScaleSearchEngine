//
//  ContainerMath.h
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/11/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#ifndef __SmallScaleSearchEngine__ContainerMath__
#define __SmallScaleSearchEngine__ContainerMath__

#include <stdio.h>

#include <vector>

double sum(const std::vector<double> & v);

double abs(const double & a);

double absSum(const std::vector<double> & a);

std::vector<double> & operator-(std::vector<double> & a, double b);

std::vector<double> & operator+(std::vector<double> & a, double b);

std::vector<double> & operator-(std::vector<double> & a, const std::vector<double> & b);

std::vector<double> & operator+(std::vector<double> & a, const std::vector<double> & b);


#endif /* defined(__SmallScaleSearchEngine__ContainerMath__) */
