//
//  getHTML.h
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/5/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#ifndef __SmallScaleSearchEngine__getHTML__
#define __SmallScaleSearchEngine__getHTML__

// #define __TEST_GETHTML_

#include <stdlib.h>
#include <assert.h>
#include <iostream>

void getHTMLHelper(FILE * f, std::string & html);
std::string getHTML(const std::string & url);


#endif /* defined(__SmallScaleSearchEngine__getHTML__) */
