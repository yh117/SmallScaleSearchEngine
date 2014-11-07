//
//  main.cpp
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/5/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#include "getHTML.h"
#include "urlFilterGenerator.h"
#include "isValidHTML.h"
#include "findLinksFromHTML.h"

#ifndef __TEST_GETHTML_
#ifndef __TEST_urlFilterGenerator_
#ifndef __TEST_isValidHTML_
#ifndef __TEST_findLinksFromHTML_

#include <iostream>
#include <vector>
#include <set>
#include <map>

using namespace std;

vector<string> findLinks (const std::string & html) {
    vector<string> links; /* or use a set? */
    // find all links in <a href="">
    // store them in a vector<string> links
}

void getGlobalUrl (const string & currUrl, vector<string> & links /* or use a set? */) {
    // for each url in links
    // if url starts with /
    // url = curr + url;
}

void linksFliter (vector<string> & links, const set<string> & allowUrlReg, const set<string> & disallowUrlReg) {
    // for each url in links
    // bool valid = false;
    // for each regex in allowUrlReg
    // if url mach regex
    // valid = true; continue;
    // if (valid) continue;
    // for each regex in disallowUrlReg
    // if rul mach regex;
    // delete url from links
}

void storeData(const string & url, const string & html, const vector<string> & links) {
    // store url and string in database
    // push url in set
    // for each url in links
    // push each link in stack
}


void crawlerHelper(string url, const set<string> & allowUrlReg, const set<string> & disallowUrlReg) {
    string html = getHTML(url);
    if (! isValid(html)) return;
    vector<string> links = findLinks(html);
    getGlobalUrl(url, links);
    linksFliter(links, allowUrlReg, disallowUrlReg);
}

void crawler(string url) {
    set<string> allowUrlReg;
    set<string> disallowUrlReg;
    // according to robot.txt, initialize allowUrlReg and disallowUrlReg
    setAllowReg(url, allowUrlReg, disallowUrlReg);
    
    
}


int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}

#endif
#endif
#endif
#endif


