#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <stdint.h>
#include <string>
#include <vector>
#include <mutex>
#include <dirent.h>
#include <stdio.h>
#include "threadpool.h"
#include <chrono>
//may be not portable
inline void cnt_items(char *path, 
                      uint32_t *count, 
                      std::vector<std::string> &names)
{
    struct dirent *d;
    DIR *dir = opendir(path);
    if (!dir)
    {     
        std::cout<<"Failed to open dir\n";
        return;
    }  
    
    while((d = readdir(dir)))
    {
        std::string name(path); 
        if (name.find("/"))
            name.append("/");
        ++(*count);
        if (strlen(d->d_name) > 2 ) 
            names.push_back(name.append(d->d_name));
    } 
    *count -= 2; //minus this and prev catalog
    closedir(dir);
}

#endif // ifndef UTILS_H
