//
//  urlFilterGenerator.h
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/5/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#ifndef __SmallScaleSearchEngine__urlFilterGenerator__
#define __SmallScaleSearchEngine__urlFilterGenerator__

// #define __TEST_urlFilterGenerator_

#include <stdlib.h>
#include <stdio.h>

#include <set>
#include <string>
#include <regex>
#include <iostream>
#include "getHTML.h"

void initDisallowUrlReg(const std::string & root, std::set<std::string> & allowUrlReg, std::set<std::string> & disallowUrlReg);

#endif /* defined(__SmallScaleSearchEngine__urlFilterGenerator__) */
