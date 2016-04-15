#ifndef EOS_RUN_H
#define EOS_RUN_H

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"

#include <vector>
#include <iostream>
#include <fstream>

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using cv::Mat;
using cv::Vec2f;
using cv::Vec3f;
using cv::Vec4f;
using std::cout;
using std::endl;
using std::vector;
using std::string;

class eos_run
{
public:
    eos_run();

    int run(boost::filesystem::path &modelfile, boost::filesystem::path &mappingsfile,
            cv::Mat &image, vector<cv::Vec2f> &landmarks_std,
            fs::path &outputfilename, fs::path &outputfilepath);
};

#endif // EOS_RUN_H
