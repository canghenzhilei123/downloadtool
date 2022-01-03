#ifndef SHA256_H
#define SHA256_H

#include <iostream>
#include <string>

using namespace std;

// https://blog.csdn.net/weixin_34341117/article/details/91750419?spm=1001.2101.3001.6650.11&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-11.no_search_link&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-11.no_search_link&utm_relevant_index=13

class Sha256
{
public:
    Sha256();
    string sha256(const string str);
    int Test();
};

#endif // SHA256_H
