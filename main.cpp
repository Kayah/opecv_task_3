#include "threadpool.h"
#include "utils.h"
using namespace cv;

void parallel_resize(Mat& image, std::vector<Mat>& dst,double coefx, double coefy)
{
    Mat out;
    resize(image, out, Size(), coefx, coefy, CV_INTER_AREA);
    dst.push_back(out);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout<<"Please enter a path to images\n";
        return 0;
    }

    char *path = argv[1];

    std::vector<Mat> images;
    
    uint32_t count_items = 0;
    std::vector<std::string> names;

    cnt_items(path, &count_items, names);

    for (uint32_t i = 0; i < count_items; i++)
    {
        images.push_back(imread(names.at(i),IMREAD_COLOR ));
    }
    
    thread_pool tp;
    std::vector<std::function<void()>> tasks;
    std::vector<Mat> res;
    
    for (size_t i = 0; i < images.size(); i++)
    {
        tasks.push_back(std::bind(parallel_resize, std::ref(images[i]), std::ref(res), 0.25, 0.25));
    }
    
    for (size_t z = 0; z < tasks.size(); z++)
    {
        tp.submit_task(tasks.at(z)); 
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(4));

    for (uint32_t i = 0; i < names.size() && i < res.size(); i++)
    {
        imwrite(names.at(i), res.at(i));
    }

    return 0;
}
