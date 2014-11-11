//
//  WebCrawler.h
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/6/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#ifndef __SmallScaleSearchEngine__WebCrawler__
#define __SmallScaleSearchEngine__WebCrawler__

#define __test_WebCrawler_

#include <stdio.h>

#include <iostream>
#include <string>
#include <set>
#include <stack>
#include <regex>
#include <map>

#include "Url.h"
#include "printContainer.h"
#include "Sql.h"


class WebCrawler {
private:
    std::string inputUrl;
    std::string rootWithSlash;
    std::string robots;
    std::string root;
    
    std::set<std::string> allowUrlReg;
    std::set<std::string> disallowUrlReg;
    
    std::set<std::string> visitedUrls;
    std::stack<std::string> unvisitedUrls;
    
    std::set<std::pair<std::string, std::string> > allLinks;
    
    const int saveSize;
    
    
    Sql db;
    
    void addSlashForRoot() {
        for (int i=0; i<inputUrl.size(); ++i) {
            if (inputUrl[i] == '^' ||
                inputUrl[i] == '$' ||
                inputUrl[i] == '\\' ||
                inputUrl[i] == '.' ||
                inputUrl[i] == '*' ||
                inputUrl[i] == '+' ||
                inputUrl[i] == '?' ||
                inputUrl[i] == '(' ||
                inputUrl[i] == ')' ||
                inputUrl[i] == '[' ||
                inputUrl[i] == ']' ||
                inputUrl[i] == '{' ||
                inputUrl[i] == '}' ||
                inputUrl[i] == '|') rootWithSlash += "\\";
            rootWithSlash += inputUrl[i];
        }
        if (inputUrl[inputUrl.size()-1] != '/') {
            inputUrl += '/';
            rootWithSlash += '/';
        }
        
        size_t slashPos = inputUrl.find("/", 7);
        if (slashPos != inputUrl.npos) {
            root = inputUrl.substr(0, slashPos+1);
        }
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
    
    void storeLinksInDB(const std::string & url, const std::set<std::string> & links) {
        std::string query;
        for (auto x:links) {
            query += "INSERT INTO LINKS (URL, LINKURL) " \
                    "VALUES ('" + url + "', '" + x + "' );" ;
        }
        db.executeSql(query.c_str());
    }
    
    void cralingHelper(Url & url) {
        const std::set<std::string> & links = url.getLinks();
        std::set<std::string> validLinks;
        
        for (auto x:links) {
            std::string link = x;
            // if this link is wired, continue to check next link
            if (linkCompleter(url, link) == false) continue;
            // check if this link is disallowed by robots.txt
            if (linkChecker(link) == false) continue;
            
            // add link into validLinks, validLinks will be stored in the database
            validLinks.insert(link);
            
            // if this link is unvisited, push it to unvisited link stack
            if (visitedUrls.find(link) == visitedUrls.end())
                unvisitedUrls.push(link);
        }
        
        storeLinksInDB(url.getUrl(), validLinks);
    }
    
    void createTables() const {
        std::string createTableVisited = "CREATE TABLE VISITED(" \
        "URL  TEXT PRIMARY KEY NOT NULL UNIQUE," \
        "HTML TEXT );" ;
        
        std::string createTableUnvisited = "CREATE TABLE UNVISITED(" \
        "URL TEXT PRIMARY KEY NOT NULL UNIQUE );" ;
        
        std::string creatTableLinks = "CREATE TABLE LINKS(" \
        "URL TEXT NOT NULL," \
        "LINKURL TEXT NOT NULL," \
        "PRIMARY KEY(URL, LINKURL) );" ;
        
        db.executeSql(createTableVisited.c_str());
        db.executeSql(createTableUnvisited.c_str());
        db.executeSql(creatTableLinks.c_str());
    }
    
    void loadDataFromDB() {
        std::string query = "SELECT URL FROM VISITED";
        db.executeSql(query.c_str(), Sql::callback);
        for (int i=0; i<Sql::result.size(); ++i) {
            visitedUrls.insert(Sql::result[i]);
        }
        Sql::result.clear();
        
        query = "SELECT URL FROM UNVISITED";
        db.executeSql(query.c_str(), Sql::callback);
        for (int i=0; i<Sql::result.size(); ++i) {
            unvisitedUrls.push(Sql::result[i]);
        }
        Sql::result.clear();
    }
    
    void dbInitialization() {
        createTables();
        loadDataFromDB();
    }
    
    void replaceAll(std::string & str, const std::string& from, const std::string& to) {
        if(from.empty())
            return;
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }
    
    void storeVisitedInDb(std::map<std::string, std::string> & temp) {
        std::string query;
        
        for (auto it = temp.begin(); it!=temp.end(); ++it) {
            replaceAll(it->second, "'", "''");
            query += "INSERT INTO VISITED (URL, HTML) " \
                     "VALUES ('" + it->first + "', '" + it->second + "' );" ;
        }
        db.executeSql(query.c_str());
        temp.clear();
    }
    
    std::set<std::string> getItemsInStack() {
        std::set<std::string> stackItems;
        while (! unvisitedUrls.empty()) {
            stackItems.insert(unvisitedUrls.top());
            unvisitedUrls.pop();
        }
        for (auto it = stackItems.rbegin(); it != stackItems.rend(); ++it) {
            unvisitedUrls.push(*it);
        }
        
        return stackItems;
    }
    
    void storeUnvisitedInDb() {
        std::string query = "DELETE FROM UNVISITED; ";
        std::set<std::string> stackItems = getItemsInStack();
        
        for (auto it = stackItems.begin(); it != stackItems.end(); ++it) {
            query += "INSERT INTO UNVISITED (URL) " \
                     "VALUES ('" + *it + "' );" ;
        }
        
        db.executeSql(query.c_str());
    }
    
    void storeDataInDB(std::map<std::string, std::string> & temp) {
        storeVisitedInDb(temp);
        storeUnvisitedInDb();
    }
    
    
public:
    WebCrawler(const std::string & _inputUrl) : inputUrl(_inputUrl), db("ssse.db"), saveSize(20) {
        // surfix "hppt://" is required
        addSlashForRoot();
        getRobots();
        initDisallowUrlReg();
    
        dbInitialization();
        
        unvisitedUrls.push(inputUrl);
    }
    
    void startCrawling() {
        std::map<std::string, std::string> temp;
        
        while (unvisitedUrls.empty() == false) {
            // get next url string in stack, initialize an Url object (after initializaion, the url is visited)
            std::string urlStr = unvisitedUrls.top();
            unvisitedUrls.pop();
            Url url(urlStr);
            
            // if url is not a valid html format, continue to visit next url in stack
            if (url.isValidHTML() == false) continue;
            
            // add current url into visitedUrls set
            auto insertSuccess = visitedUrls.insert(urlStr);
            
            if (insertSuccess.second == false) continue;
                
            temp.insert(std::pair<std::string, std::string>(urlStr, url.getHTML()));
            
            // check if the links in this page are allowed and visited
            // push valid and unvisited links into unvisited links stack
            cralingHelper(url);
            
            if (temp.size() == saveSize) {
                storeDataInDB(temp);
                // print<std::set<std::string> >(visitedUrls);
            }
            
        }
        storeDataInDB(temp);
    }
    
    const std::set<std::string> & getVisitedUrls() {
        return visitedUrls;
    }
    
    const std::set<std::pair<std::string, std::string> > getLinks() {
        std::string query = "SELECT * FROM LINKS;";
        db.executeSql(query.c_str(), Sql::callback);
        
        assert(Sql::result.size() % 2 == 0);
        
        for (int i=0; i<Sql::result.size(); i=i+2) {
            allLinks.insert(std::pair<std::string, std::string>(Sql::result[i], Sql::result[i+1]));
        }
        
        return allLinks;
    }
    
};




#endif /* defined(__SmallScaleSearchEngine__WebCrawler__) */
