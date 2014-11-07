//
//  getHTML.cpp
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/5/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#include "getHTML.h"

// store all the content in f to html
void getHTMLHelper(FILE * f, std::string & html) {
    char * line = NULL;
    size_t size;
    
    while (getline(&line, &size, f) != -1) {
        html += line;
    }
    
    free(line);
}

std::string getHTML(const std::string & url) {
    // use console command "curl url" to get html text
    std::string commend = "curl " + url;
    
    // excute the command, store the url in FILE * f
    FILE * f = popen(commend.c_str(), "r");
    
    // the url must be able to be visited
    assert(f != NULL);
    
    // get html test from f
    std::string html;
    getHTMLHelper(f, html);
    
    pclose(f);
    
    return html;
}

#ifdef __TEST_GETHTML_

int main(int argc, char ** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./exe url" << std::endl;
    }
    std::cout << getHTML(argv[1]) << std::endl;
    return EXIT_SUCCESS;
}

#endif
