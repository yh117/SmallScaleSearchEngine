//
//  printContainer.h
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/6/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#ifndef __SmallScaleSearchEngine__printContainer__
#define __SmallScaleSearchEngine__printContainer__

#include <stdio.h>
#include <iostream>

template<typename T>
void print(T container) {
    int i = 1;
    for (auto x:container) std::cout << i++ << ": " << x << std::endl;
}

#endif /* defined(__SmallScaleSearchEngine__printContainer__) */
