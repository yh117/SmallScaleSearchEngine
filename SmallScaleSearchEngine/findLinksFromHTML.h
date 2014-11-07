//
//  findLinksFromHTML.h
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/6/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#ifndef __SmallScaleSearchEngine__findLinksFromHTML__
#define __SmallScaleSearchEngine__findLinksFromHTML__

#define __TEST_findLinksFromHTML_

#include <stdio.h>
#include <string>
#include <set>
#include <iostream>
#include <regex>

#include "getHTML.h"

std::set<std::string> findLinksFromHTML(const std::string & root, const std::string & curr, std::string html);

#endif /* defined(__SmallScaleSearchEngine__findLinksFromHTML__) */
