//
//  findLinksFromHTML.cpp
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/6/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#include "findLinksFromHTML.h"

bool linkCompleter(const std::string & root, const std::string & curr, std::string & link) {
    if (link.find("http") == 0) return true;
    
    if (link.size() > 0 && link[0] == '/') {
        if (root[root.size()-1] == '/') link = root + link.substr(1);
        else link = root + link;
        return true;
    }
    
    else if (curr[curr.size()-1] == '/') {
        link = curr + link;
        return true;
    }
    
    return false;
}

std::set<std::string> findLinksFromHTML(const std::string & root, const std::string & curr, std::string html) {
    std::set<std::string> links;
    
    // initialize the pattern to match "<a href="">"
    std::regex pattern("<");
    
    // object that will contain the sequence of sub-matches
    std::smatch matches;
    
    // initialize the pattern to match "<meta name="robots" content="noindex">"
    pattern = std::regex("< *a[^>]+href *= *\" *([^#\"][^\" ]*)\"");
    int i=1;
    while (std::regex_search(html, matches, pattern)) {
        html = matches.suffix().str();
        
        std::string tmp = matches.str(1);
        if (tmp.empty()) continue;
        if (tmp == "javascript:;") continue;
        if (tmp.find("mailto") == 0) continue;
        
        if (linkCompleter(root, curr, tmp))
        std::cout << i++ << ": " << tmp << std::endl;
    }
    
    return links;
}


#ifdef __TEST_findLinksFromHTML_

#include "WebCrawler.h"

int main(int argc, char ** argv) {
    /*
    char * root = "http://www.ece.duke.edu";
    std::string html = getHTML(root);
    // std::cout << html << std::endl;
    findLinksFromHTML(root, root, html);
     */
    
    WebCrawler wc("http://www.ece.duke.edu/");
    
    wc.startCrawling();
    
    
    
    return EXIT_SUCCESS;
}

#endif