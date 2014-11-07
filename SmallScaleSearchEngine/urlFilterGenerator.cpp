//
//  urlFilterGenerator.cpp
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/5/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//


#include "urlFilterGenerator.h"

#ifndef __TEST_GETHTML_

// add escape sequence '/' before ^ $ \ . * + ? ( ) [ ] { } |, for more information, see http://www.cplusplus.com/reference/regex/ECMAScript/
std::string addSlashForRoot(const std::string & str) {
    std::string ans;
    for (int i=0; i<str.size(); ++i) {
        if (str[i] == '^' ||
            str[i] == '$' ||
            str[i] == '\\' ||
            str[i] == '.' ||
            str[i] == '*' ||
            str[i] == '+' ||
            str[i] == '?' ||
            str[i] == '(' ||
            str[i] == ')' ||
            str[i] == '[' ||
            str[i] == ']' ||
            str[i] == '{' ||
            str[i] == '}' ||
            str[i] == '|') ans += "\\";
        ans += str[i];
    }
    return ans;
}

std::string addSlashForMatch(const std::string & match) {
    std::string ans;
    size_t size = match.size();
    for (int i=0; i<size; ++i) {
        if (match[i] == '*') ans += '.';
        if (match[i] == '^' ||
            (match[i] == '$' && i != size-1) ||
            match[i] == '\\' ||
            match[i] == '.' ||
            match[i] == '+' ||
            match[i] == '?' ||
            match[i] == '(' ||
            match[i] == ')' ||
            match[i] == '[' ||
            match[i] == ']' ||
            match[i] == '{' ||
            match[i] == '}' ||
            match[i] == '|') ans += "\\";
        ans += match[i];
    }
    if (size == 0 || match[size-1] != '$') ans += ".*";
    return ans;
}

// mode = 1, initialize allowUrlReg
// mode = 0, initialize disallowUrlReg
void initDisallowUrlRegHelper(const std::string & rootWithSlash, std::string robots, std::set<std::string> & urlReg, int mode) {
    
    // initialize the pattern to match the lines that contain "Disallow: /" or "Allow: /"
    std::regex pattern;
    if (mode) pattern = std::regex("Allow: /(.*)");
    else pattern = std::regex("Disallow: /(.*)");
    
    // object that will contain the sequence of sub-matches
    std::smatch matches;
    
    // regex search all matches of pattern in robots
    while (std::regex_search(robots, matches, pattern)) {
        // make matches into regex string and add them (with root url) into a set
        urlReg.insert(rootWithSlash + addSlashForMatch(matches.str(1)));
        // searh for the next match
        robots = matches.suffix().str();
    }
    
}

void initDisallowUrlReg(const std::string & root, std::set<std::string> & allowUrlReg, std::set<std::string> & disallowUrlReg) {
    
    std::string robots;
    std::string rootWithSlash;
    // consider two cases: root ends with '/' and not with '/'
    if (root[root.size()-1] == '/') rootWithSlash = root + "robots.txt";
    else rootWithSlash = rootWithSlash = root + "/robots.txt";
    robots = getHTML(rootWithSlash);
    
    // make root as a regex
    rootWithSlash = addSlashForRoot(root);
    
    allowUrlReg.insert(rootWithSlash + ".*");
    
    // add rules in robots.txt into disallowUrlReg and allowUrlReg as regular expressiion string
    initDisallowUrlRegHelper(rootWithSlash, robots, disallowUrlReg, 0);
    initDisallowUrlRegHelper(rootWithSlash, robots, allowUrlReg, 1);
}


#ifdef __TEST_urlFilterGenerator_

int main(int argc, char ** argv) {
    std::set<std::string> allowUrlReg;
    std::set<std::string> disallowUrlReg;
    
    initDisallowUrlReg(argv[1], allowUrlReg, disallowUrlReg);
    
    for (auto x:allowUrlReg) std::cout << x << std::endl;
    for (auto x:disallowUrlReg) std::cout << x << std::endl;

    std::cout << addSlashForMatch("images/dogs.jpg") << std::endl;
    std::cout << addSlashForMatch("*.gif$$") << std::endl;
    std::cout << addSlashForMatch("*?") << std::endl;
    std::cout << addSlashForMatch("*.xls$") << std::endl;
    
    return EXIT_SUCCESS;
}
 

#endif
#endif
