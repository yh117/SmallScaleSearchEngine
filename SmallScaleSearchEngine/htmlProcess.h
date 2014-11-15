#ifndef __SmallScaleSearchEngine__HtmlProcess__
#define __SmallScaleSearchEngine__HtmlProcess__
//#include <tidy.h>
//#include <buffio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <regex>
using namespace std;
class HtmlProcess{
private:
    string CleanHtml(const std::string & html){
        // for test case, write html to file, run command
        char* line;
        size_t size;
        string cleanHtml;
        ofstream out("temp.html");
        out << html;
        out.close();
        // run tidy command
        FILE* fp = popen("tidy -asxml temp.html 2>fixed.xml","r");
        while((getline(&line,&size,fp))!= -1){
            cout<< line<<endl;
            cleanHtml+= line;
        }
        free(line);
        pclose(fp);
        return cleanHtml;
    }
    // extract title
    string GetTitle(const string & html){
        regex title_reg("<title>.*</title>");
        regex title_rep("<title>|</title>");
        string result = RegHelper(html,title_reg,title_rep);
        return result;

    }
    // extract content between tags, example >content<
    string GetContent(const string &html){
        regex content_reg(">[^<]+[a-zA-Z0-9]+[^<]*<");
        regex content_rep("<|>");
        string result = RegHelper(html,content_reg,content_rep);
        return result;
    }

    // extract the content between <h> and </h>
    string GetHead(const string & html){
        regex  paragraph_reg("<h.*>.*</h.*>");
        regex paragraph_rep("<h[^<>]*>|</h[^<>]*>");
        string result = RegHelper(html,paragraph_reg,paragraph_rep);
        return result;
    }
    string RegHelper(const string & html, regex& reg, regex& rep){
        string content;
        sregex_iterator content_itor(html.begin(),html.end(),reg);
        sregex_iterator it_end;
        while(content_itor != it_end){
            content += (*content_itor).str();
            content_itor++;
        }
        string result;
        regex_replace (back_inserter(result), content.begin(), content.end(),rep, " ");
        return result;
    }
public:
    HtmlProcess(){}
    string cleanHtml(const string & html){
        return CleanHtml(html);
    }
	string getTitle(const string & html){
		return GetTitle(html);
	}
    string getContent(const string & html){
        return GetContent(html);
    }
    string getHead(const string & html){
        return GetHead(html);
    }
};
#endif