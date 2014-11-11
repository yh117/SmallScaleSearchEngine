//
//  main.cpp
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/5/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//


#include "WebCrawler.h"


#include <unordered_map>
#include <unordered_set>
#include <algorithm>


double sum(const std::vector<double> & v) {
    double ans = 0;
    for (int i=0; i<v.size(); ++i) ans += v[i];
    return ans;
}

double abs(const double & a) {
    return a > 0 ? a : -a;
}

double absSum(const std::vector<double> & a) {
    double sum;
    for (int i=0; i<a.size(); ++i) sum += abs(a[i]);
    return sum;
}

std::vector<double> & operator-(std::vector<double> & a, double b) {
    for (int i=0; i<a.size(); ++i) a[i] -= b;
    return a;
}

std::vector<double> & operator+(std::vector<double> & a, double b) {
    for (int i=0; i<a.size(); ++i) a[i] += b;
    return a;
}

std::vector<double> & operator-(std::vector<double> & a, const std::vector<double> & b) {
    for (int i=0; i<a.size(); ++i) a[i] -= b[i];
    return a;
}

std::vector<double> & operator+(std::vector<double> & a, const std::vector<double> & b) {
    for (int i=0; i<a.size(); ++i) a[i] += b[i];
    return a;
}

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

int main(void) {
    
    WebCrawler wc("http://www.ece.duke.edu/");
    
    wc.startCrawling();
    
    // after crawling finished, get all the urls
    auto urls = wc.getVisitedUrls();
    auto links = wc.getLinks();
    
    // assigned an int index (from 0) to urls, store the <url, index> pair to unordered_map urlDict
    std::unordered_map<std::string, int> urlDict;
    std::unordered_map<int, std::string> indexDict;
    int i = 0;
    for (auto x : urls) {
        urlDict.insert(std::pair<std::string, int>(x, i));
        indexDict.insert(std::pair<int, std::string>(i, x));
        ++i;
    }
    
    // for (auto x : urlDict) std::cout << x.second << ": " << x.first << std::endl;
    
    // store links as ==> <index of url, a set of indexes of all links in that url> pair
    std::unordered_map<int, std::unordered_set<int> > linkToMatrix, linkFromMatrix;
    // initialize linkMatrix
    for (int i=0; i<urls.size(); ++i) {
        linkToMatrix.insert(std::pair<int, std::unordered_set<int> >(i, std::unordered_set<int>()));
    }
    linkFromMatrix = linkToMatrix;
    for (auto x : links) {
        if (urlDict.find(x.first) == urlDict.end() || urlDict.find(x.second) == urlDict.end()) continue;
        
        linkToMatrix[urlDict[x.first]].insert(urlDict[x.second]);
        linkFromMatrix[urlDict[x.second]].insert(urlDict[x.first]);
    }
    
    // N is the size (number) of urls
    int N = (int)linkToMatrix.size();
    
    std::vector<int> M(N);
    for (auto x : linkToMatrix) {
        M[x.first] = (int)x.second.size();
    }
    
    std::vector<double> pr = pageRank(N, M, linkFromMatrix);
    
    std::map<double, std::string> urlRank;
    
    for (int i=0; i<pr.size(); ++i) {
        urlRank.insert(std::pair<double, std::string>(pr[i], indexDict[i]));
    }
    
    for (auto it=urlRank.rbegin(); it!=urlRank.rend(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    
    return EXIT_SUCCESS;
}