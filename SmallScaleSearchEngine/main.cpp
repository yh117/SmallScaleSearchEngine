//
//  main.cpp
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/5/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#include "WebCrawler.h"

int main(int argc, char ** argv) {
    /*
     char * root = "http://www.ece.duke.edu/faculty";
     std::string html = getHTML(root);
     // std::cout << html << std::endl;
     findLinksFromHTML(root, root, html);
     */
    
    WebCrawler wc("http://www.ece.duke.edu/");
    
    wc.startCrawling();
    
    
    
    return EXIT_SUCCESS;
}