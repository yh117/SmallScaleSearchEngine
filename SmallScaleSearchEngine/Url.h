//
//  Url.h
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/6/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#ifndef __SmallScaleSearchEngine__Url__
#define __SmallScaleSearchEngine__Url__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <iostream>
#include <string>
#include <regex>
#include <set>

#include "printContainer.h"


class Url {
private:
    std::string url;
    std::string html;
    
    std::set<std::string> links;
    
    void getHTMLHelper(FILE * f) {
        char * line = NULL;
        size_t size;
        
        while (getline(&line, &size, f) != -1) {
            html += line;
        }
        
        free(line);
    }
    
    void HTMLGetter() {
        // use console command "curl url" to get html text
        std::string commend = "curl " + url + " 2>/dev/null";
        
        // excute the command, store the url in FILE * f
        FILE * f = popen(commend.c_str(), "r");
        
        // the url must be able to be visited
        assert(f != NULL);
        
        // get html test from f
        getHTMLHelper(f);
        
        pclose(f);
    }
    
    void findLinksFromHTML() {
        // initialize the pattern to match "<a href="">"
        std::regex pattern("<");
        
        // object that will contain the sequence of sub-matches
        std::smatch matches;
        
        // initialize the pattern to match "<meta name="robots" content="noindex">"
        pattern = std::regex("< *a[^>]+href *= *\" *([^#\"][^\" ]*)\"");
        while (std::regex_search(html, matches, pattern)) {
            html = matches.suffix().str();
            
            // 3 cases below are not considered as valid url
            std::string tmp = matches.str(1);
            if (tmp.empty()) continue;
            if (tmp == "javascript:;") continue;
            if (tmp.find("mailto") == 0) continue;
            
            links.insert(tmp);
        }
    }
    
public:
    Url(const std::string & _url) : url(_url) {
        HTMLGetter();
    }
    
    const std::string & getHTML() const {
        return html;
    }
    
    const std::string & getUrl() const {
        return url;
    }
    
    bool isValidHTML () const {
        // initialize the pattern to match "<html>"
        std::regex pattern("<[Hh][Tt][Mm][Ll]");
        
        // object that will contain the sequence of sub-matches
        std::smatch matches;
        
        // if match return true, otherwise return false
        if (! std::regex_search(html, matches, pattern)) return false;
        
        // initialize the pattern to match "<meta name="robots" content="noindex">"
        pattern = std::regex("< *meta[^>]+noindex");
        if(std::regex_search(html, matches, pattern)) return false;
        
        return true;
    }
    
    const std::set<std::string> & getLinks()  {
        
        findLinksFromHTML();
        
        // print<std::set<std::string> >(links);
        
        return links;
    }
    
    
};





#endif /* defined(__SmallScaleSearchEngine__Url__) */
