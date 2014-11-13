//
//  main.cpp
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/5/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#include <unordered_map>
#include <unordered_set>

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <ios>
#include <sstream>
#include <iomanip>

#include "WebCrawler.h"
#include "ContainerMath.h"
#include "Sql.h"
#include "porter2_stemmer.h"



std::vector<double> pageRank(int N, const std::vector<int> & M, const std::unordered_map<int, std::unordered_set<int> > & linkFromMatrix) {
    // initialize the page rank of all urls as 1
    std::vector<double> pageRank(N, 1.0/N);
    double error;
    double beta = 0.85;
    // PageRank algorithm
    std::vector<double> tmp(N);
    do {
        for (int i=0; i<N; ++i) tmp[i] = 0;
        
        for (auto x : linkFromMatrix) {
            for (auto y : x.second) {
                tmp[x.first] += beta * pageRank[y] / M[y];
            }
        }
        
        double prSum = sum(tmp);
        
        tmp = tmp + ((1 - prSum) / N);
        
        pageRank = pageRank - tmp;
        
        error = absSum(pageRank);
        
        pageRank = tmp;
        
    }while (error > 1e-8);
    
    return pageRank;
}

// initialize containers for PageRank calculation
void pageRankInit(int N,
                  const std::map<std::string, std::string> & urls,
                  const std::set<std::pair<std::string, std::string> > & links,
                  std::unordered_map<std::string, int> & urlDict,
                  std::unordered_map<int, std::string> & indexDict,
                  std::unordered_map<int, std::unordered_set<int> > & linkToMatrix,
                  std::unordered_map<int, std::unordered_set<int> > & linkFromMatrix,
                  std::vector<int> & M)
{
    int i = 0;
    for (auto x : urls) {
        urlDict.insert(std::pair<std::string, int>(x.first, i));
        indexDict.insert(std::pair<int, std::string>(i, x.first));
        ++i;
    }
    
    
    // initialize linkToMatrix, linkFromMatrix
    for (int i=0; i<N; ++i) {
        linkToMatrix.insert(std::pair<int, std::unordered_set<int> >(i, std::unordered_set<int>()));
    }
    linkFromMatrix = linkToMatrix;
    
    // transform links into linkFromMatrix and linkToMatrix
    for (auto x : links) {
        if (urlDict.find(x.first) == urlDict.end() || urlDict.find(x.second) == urlDict.end()) continue;
        
        linkToMatrix[urlDict[x.first]].insert(urlDict[x.second]);
        linkFromMatrix[urlDict[x.second]].insert(urlDict[x.first]);
    }
    
    for (auto x : linkToMatrix) {
        M[x.first] = (int)x.second.size();
    }
}

// print PageRank result from the most popular to the least popular
void printPageRank(const std::vector<double> & pr, std::unordered_map<int, std::string> & indexDict) {
    std::set<std::pair<double, std::string> > urlRank;
    
    for (int i=0; i<pr.size(); ++i) {
        urlRank.insert(std::pair<double, std::string>(pr[i], indexDict[i]));
    }
    
    for (auto it=urlRank.rbegin(); it!=urlRank.rend(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}

template<typename T>
void loadDataHelper(T & container, std::string tableName) {
    std::string query = "SELECT * FROM " + tableName;
    Sql db("ssse.db");
    
    db.executeSql(query.c_str(), Sql::callback);
    
    assert(Sql::result.size() % 2 == 0);
    
    for (int i=0; i<Sql::result.size(); i=i+2) {
        container.insert(std::pair<std::string, std::string>(Sql::result[i], Sql::result[i+1]));
    }
    
    Sql::result.clear();
}

void html2text(std::string & html) {
    std::ofstream ofs("tmp.html", std::ios::out | std::ios::trunc);
    ofs << html;
    ofs.close();
    
    FILE * f = popen("/usr/local/bin/elinks -dump tmp.html", "r");
    char * line = NULL;
    size_t size;
    html = "";
    while(getline(&line, &size, f) != -1){
        html += line;
    }
    free(line);
    pclose(f);
}

void loadDataFromDB(std::map<std::string, std::string> & urls, std::set<std::pair<std::string, std::string> > & links) {
    loadDataHelper<std::map<std::string, std::string> >(urls, "VISITED");
    loadDataHelper<std::set<std::pair<std::string, std::string> > >(links, "LINKS");
    
    for (auto it = urls.begin(); it != urls.end(); ++it) {
        html2text(it->second);
    }
}

void stemHtml(std::map<std::string, std::string> &   urls,
              std::unordered_map<int, std::string> & indexDict,
              std::vector<std::string> &             stemmedUrls)
{
    for (int i=0; i<indexDict.size(); ++i) {
        stemmedUrls[i] = urls[indexDict[i]];
        Porter2Stemmer::stem(stemmedUrls[i]);
    }
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

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6) {
    std::ostringstream out;
    out << std::setprecision(n) << a_value;
    return out.str();
}

void buildDatabase(int N,
                   std::unordered_map<int, std::string> & indexDict,
                   std::map<std::string, std::string> & urls,
                   std::vector<std::string> & stemmedUrls,
                   const std::vector<double> & pr,
                   std::unordered_map<int, std::unordered_set<int> > & linkToMatrix)
{                       // create table urls
    Sql db("db.db");
    std::string query = "CREATE TABLE URLS(" \
                        "ID INTEGER PRIMARY KEY NOT NULL," \
                        "URL TEXT NOT NULL UNIQUE," \
                        "CONTENT TEXT NOT NULL," \
                        "STEMMEDCONTENT TEXT NOT NULL," \
                        "PAGERANK REAL NOT NULL );";
                        // create table links
    db.executeSql(query.c_str());
    query             = "CREATE TABLE LINKS(" \
                        "ID INTEGER NOT NULL," \
                        "LINKID INTEGER NOT NULL," \
                        "PRIMARY KEY(ID, LINKID) );";
    db.executeSql(query.c_str());
    
    for (int i=0; i<N; ++i) {
        
        replaceAll(urls[indexDict[i]], "'", "''");
        replaceAll(stemmedUrls[i], "'", "''");
        query = "INSERT INTO URLS (ID, URL, CONTENT, STEMMEDCONTENT, PAGERANK) VALUES ( " + std::to_string(i) + ", '" + indexDict[i] + "', '" + urls[indexDict[i]] + "', '" + stemmedUrls[i] + "', " + to_string_with_precision(pr[i], 10) + " );";
        db.executeSql(query.c_str());
        
        query = "";
        for (auto y : linkToMatrix[i]) {
            query += "INSERT INTO LINKS (ID, LINKID) VALUES (" + std::to_string(i) + ", " + std::to_string(y) + "); ";
        }
        db.executeSql(query.c_str());
        
    }
}

void postProcesser() {
    // read database, get urls and links
    std::map<std::string, std::string> urls;
    std::set<std::pair<std::string, std::string> > links;
    loadDataFromDB(urls, links);

    // N is the size (number) of urls
    int N = (int)urls.size();
    
    // assigned an int index (from 0) to urls, store the <url, index> pair to unordered_map urlDict
    std::unordered_map<std::string, int> urlDict;
    std::unordered_map<int, std::string> indexDict;
    
    // store links as linkToMatrix, linkFromMatrix
    std::unordered_map<int, std::unordered_set<int> > linkToMatrix, linkFromMatrix;
    
    // variable M is the matrix M in page rank algorithm, M[i] = number of links from urls[i] to other urls
    std::vector<int> M(N);
    
    // initialize variables for PageRank calculation
    pageRankInit(N, urls, links, urlDict, indexDict, linkToMatrix, linkFromMatrix, M);

    // pr[i] = pagerank result for urls[i]
    std::vector<double> pr = pageRank(N, M, linkFromMatrix);
    
    // print PageRank result
    printPageRank(pr, indexDict);
    
    // std::cout << urls[indexDict[0]] << std::endl;
    
    // stem html
    std::vector<std::string> stemmedUrls(N);
    stemHtml(urls, indexDict, stemmedUrls);
    
    // write PageRank result into database
    buildDatabase(N, indexDict, urls, stemmedUrls, pr, linkToMatrix);
    
}

void crawlerUrl(const std::string & url) {
    WebCrawler wc(url);
    wc.startCrawling();
}

int main(void) {
    
    // crawlerUrl("http://www.ece.duke.edu/");
    
    postProcesser();
    
    return EXIT_SUCCESS;
}
