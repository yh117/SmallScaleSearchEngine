//
//  isValidHTML.cpp
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/6/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#include "isValidHTML.h"

bool isValidHTML (const std::string & html) {
    
    // initialize the pattern to match "<html>"
    std::regex pattern("<[Hh][Tt][Mm][Ll]>");
    
    // object that will contain the sequence of sub-matches
    std::smatch matches;
    
    // if match return true, otherwise return false
    if (! std::regex_search(html, matches, pattern)) return false;
    
    // initialize the pattern to match "<meta name="robots" content="noindex">"
    pattern = std::regex("< *meta[^>]+noindex");
    if(std::regex_search(html, matches, pattern)) return false;
    
    return true;
}


#ifdef __TEST_isValidHTML_

int main(int argc, char ** argv) {
    std::string html1 = "fiasd<html>asdfjlsj";
    std::string html2 = "asjdifo<HTML>sjdifo";
    std::string html3 = "<html> < meta     name = \"robots\" content  = \"  noindex  \"  >";
    std::string html4 = getHTML(argv[1]);
    
    std::cout << html1 << ": is " << (isValidHTML(html1) ? "valid" : "not valid") << std::endl;
    std::cout << html2 << ": is " << (isValidHTML(html2) ? "valid" : "not valid") << std::endl;
    std::cout << html3 << ": is " << (isValidHTML(html3) ? "valid" : "not valid") << std::endl;
    std::cout << argv[1] << ": is " << (isValidHTML(html4) ? "valid" : "not valid") << std::endl;
    
    return EXIT_SUCCESS;
}

#endif