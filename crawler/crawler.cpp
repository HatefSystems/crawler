// Copyright 2013 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: jdtang@google.com (Jonathan Tang)
//
// Gets the cleantext of a page.

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include "gumbo.h"
#include <regex>

using namespace std;

static string cleantext(GumboNode* node) {
    if (node->type == GUMBO_NODE_TEXT) {
        return string(node->v.text.text);
    }
    else if (node->type == GUMBO_NODE_ELEMENT &&
        node->v.element.tag != GUMBO_TAG_SCRIPT &&
        node->v.element.tag != GUMBO_TAG_STYLE) {
        string contents = "";
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            const string text = cleantext((GumboNode*)children->data[i]);
            if (i != 0 && !text.empty()) {
                contents.append(" ");
            }
            contents.append(text);
        }
        return contents;
    }
    else {
        return "";
    }
}



int main(int argc, char** argv) {
    //if (argc != 2) {
    //    cout << "Usage: clean_text <html filename>\n";
    //    exit(EXIT_FAILURE);
    //}
    const char* filename = argv[1];

    ifstream in(filename, ios::in | ios::binary);
    if (!in) {
        cout << "File " << filename << " not found!\n";
        exit(EXIT_FAILURE);
    }

    string contents;
    in.seekg(0, ios::end);
    contents.resize(in.tellg());
    in.seekg(0, ios::beg);
    in.read(&contents[0], contents.size());
    in.close();

    GumboOutput* output = gumbo_parse(contents.c_str());
    cout << cleantext(output->root) << endl;




    // Regular expression to match the <title> tag and its content
    regex title_regex("<title>\\s+(.*?)\\s+</title>");
    smatch match;

    // Find the first match
    if (regex_search(contents, match, title_regex)) {
        // Extract the title
        string title = match[1].str();
        cout << "Title: " << title << endl;
    }
    else {
        cout << "Title not found." << endl;
    }


    // Regular expression to match the description tag and its content
    regex regex_pattern("<meta\\s+name=\"description\"\\s+content=\"(.*?)\"");
    smatch matches;

    if (regex_search(contents, matches, regex_pattern)) {
        string description = matches[1].str();
        cout << "Meta description: " << description << endl;
    }
    else {
        cout << "Meta description not found." << endl;
    }
    
    return 1;

}