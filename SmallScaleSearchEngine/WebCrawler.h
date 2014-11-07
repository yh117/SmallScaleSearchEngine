//
//  WebCrawler.h
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/6/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#ifndef __SmallScaleSearchEngine__WebCrawler__
#define __SmallScaleSearchEngine__WebCrawler__

#include <stdio.h>

#include <iostream>
#include <string>
#include <set>
#include <stack>
#include <regex>

#include "Url.h"
#include "printContainer.h"

class WebCrawler {
private:
    std::string root;
    std::string rootWithSlash;
    std::string robots;
    
    std::set<std::string> allowUrlReg;
    std::set<std::string> disallowUrlReg;
    
    std::set<std::string> visitedUrls;
    std::stack<std::string> unvisitedUrls;
    
    void addSlashForRoot() {
        for (int i=0; i<root.size(); ++i) {
            if (root[i] == '^' ||
                root[i] == '$' ||
                root[i] == '\\' ||
                root[i] == '.' ||
                root[i] == '*' ||
                root[i] == '+' ||
                root[i] == '?' ||
                root[i] == '(' ||
                root[i] == ')' ||
                root[i] == '[' ||
                root[i] == ']' ||
                root[i] == '{' ||
                root[i] == '}' ||
                root[i] == '|') rootWithSlash += "\\";
            rootWithSlash += root[i];
        }
        if (root[root.size()-1] != '/') rootWithSlash += '/';
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
    void initDisallowUrlRegHelper(std::set<std::string> & urlReg, int mode) {
        
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
    
    void getRobots() {
        // consider two cases: root ends with '/' and ends without '/'
        std::string robotsUrlStr;
        if (root[root.size()-1] == '/') robotsUrlStr = root + "robots.txt";
        else robotsUrlStr = root + "/robots.txt";
        
        Url robotsUrl(robotsUrlStr);
        robots = robotsUrl.getHTML();
        
        // std::cout << robots << std::endl;
    }
    
    void initDisallowUrlReg() {
        
        // add rules in robots.txt into disallowUrlReg and allowUrlReg as regular expressiion string
        initDisallowUrlRegHelper(disallowUrlReg, 0);
        initDisallowUrlRegHelper(allowUrlReg, 1);
        
        rootWithSlash += ".*";
        
        // print<std::set<std::string> >(disallowUrlReg);
        // print<std::set<std::string> >(allowUrlReg);
    }
    
    bool linkCompleter(const Url & url, std::string & link) {
        if (link.find("http") == 0) return true;
        
        if (link.size() > 0 && link[0] == '/') {
            if (root[root.size()-1] == '/') link = root + link.substr(1);
            else link = root + link;
            return true;
        }
        
        std::string urlStr = url.getUrl();
        if (urlStr[urlStr.size()-1] == '/') {
            link = urlStr + link;
            return true;
        }
        
        return false;
    }
    
    bool linkChecker(const std::string & link) {
        std::regex pattern(rootWithSlash);
        std::smatch matches;
        // if link is an url outside of root, return false
        if (std::regex_match(link, matches, pattern) == false) return false;
        
        for (auto x:allowUrlReg) {
            pattern = std::regex(x);
            // if link is a allow url specified by robots.txt, return true
            if (std::regex_match(link, matches, pattern)) return true;
        }
        
        for (auto x:disallowUrlReg) {
            pattern = std::regex(x);
            // if link is a disallowed url specified by robots.txt, return false
            if (std::regex_match(link, matches, pattern)) return false;
        }
        
        return true;
    }
    
    void cralingHelper(Url & url) {
        const std::set<std::string> & links = url.getLinks();
        
        for (auto x:links) {
            std::string link = x;
            // if this link is wired, continue to check next link
            if (linkCompleter(url, link) == false) continue;
            // check if this link is disallowed by robots.txt
            if (linkChecker(link) == false) continue;
            
            // if this link is unvisited, push it to unvisited link stack
            if (visitedUrls.find(link) == visitedUrls.end())
                unvisitedUrls.push(link);
        }
        
        
    }
    
public:
    WebCrawler(const std::string & _root) : root(_root) {
        // surfix "hppt://" is required
        addSlashForRoot();
        getRobots();
        initDisallowUrlReg();
        
        unvisitedUrls.push(root);
    }
    
    void startCrawling() {
        while (unvisitedUrls.empty() == false) {
            // get next url string in stack, initialize an Url object (after initializaion, the url is visited)
            std::string urlStr = unvisitedUrls.top();
            Url url(urlStr);
            unvisitedUrls.pop();
            
            // add current url into visitedUrls set
            visitedUrls.insert(urlStr);
            
            // if url is not a valid html format, continue to visit next url in stack
            if (url.isValidHTML() == false) continue;
            
            // check if the links in this page are allowed and visited
            // push valid and unvisited links into unvisited links stack
            cralingHelper(url);
            
            if (visitedUrls.size() % 100 == 0)
                print<std::set<std::string> >(visitedUrls);
            
        }
        
    }
    
    
    
    
    
    
    
    
};




#endif /* defined(__SmallScaleSearchEngine__WebCrawler__) */
