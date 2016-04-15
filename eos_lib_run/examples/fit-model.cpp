
#include <eos_lib/eos_run.h>

/**
 * Reads an ibug .pts landmark file and returns an ordered vector with
 * the 68 2D landmark coordinates.
 *
 * @param[in] filename Path to a .pts file.
 * @return An ordered vector with the 68 ibug landmarks.
 */
std::vector<cv::Vec2f> read_pts_landmarks(std::string filename)
{
	using std::getline;
	using cv::Vec2f;
	using std::string;
    std::vector<cv::Vec2f> landmarks;
	landmarks.reserve(68);

	std::ifstream file(filename);
	if (!file.is_open()) {
		throw std::runtime_error(string("Could not open landmark file: " + filename));
	}

	string line;
	// Skip the first 3 lines, they're header lines:
	getline(file, line); // 'version: 1'
	getline(file, line); // 'n_points : 68'
	getline(file, line); // '{'

	while (getline(file, line))
	{
		if (line == "}") { // end of the file
			break;
		}
		std::stringstream lineStream(line);
        cv::Vec2f landmark;
        if (!(lineStream >> landmark[0] >> landmark[1])) {
			throw std::runtime_error(string("Landmark format error while parsing the line: " + line));
		}

		landmarks.emplace_back(landmark);
	}
	return landmarks;
}

/**
 * This app demonstrates estimation of the camera and fitting of the shape
 * model of a 3D Morphable Model from an ibug LFPW image with its landmarks.
 *
 * First, the 68 ibug landmarks are loaded from the .pts file and converted
 * to vertex indices using the LandmarkMapper. Then, an affine camera matrix
 * is estimated, and then, using this camera matrix, the shape is fitted
 * to the landmarks.
 */
int main(int argc, char *argv[])
{
    fs::path modelfile, imagefile, landmarksfile, mappingsfile, outputfilename, outputfilepath;
	try {
		po::options_description desc("Allowed options");
		desc.add_options()
            ("help,h", "display the help message")
            ("model,m", po::value<fs::path>(&modelfile)->required()->default_value("../share/sfm_shape_3448.bin"), "a Morphable Model stored as cereal BinaryArchive")
            ("image,i", po::value<fs::path>(&imagefile)->required()->default_value("data/image_0010.png"), "an input image")
            ("landmarks,l", po::value<fs::path>(&landmarksfile)->required()->default_value("data/image_0010.pts"), "2D landmarks for the image, in ibug .pts format")
            ("mapping,p", po::value<fs::path>(&mappingsfile)->required()->default_value("../share/ibug2did.txt"), "landmark identifier to model vertex number mapping")
            ("outputfilename,o", po::value<fs::path>(&outputfilename)->required()->default_value("out_fuck.obj"), "basename for the output rendering and obj files")
            ("outputfilepath,o", po::value<fs::path>(&outputfilepath)->required()->default_value("data/"), "basename for the output rendering and obj files")
            ;
		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
		if (vm.count("help")) {
			cout << "Usage: fit-model [options]" << endl;
			cout << desc;
			return EXIT_SUCCESS;
		}
		po::notify(vm);
	}
	catch (const po::error& e) {
		cout << "Error while parsing command-line arguments: " << e.what() << endl;
		cout << "Use --help to display a list of options." << endl;
		return EXIT_SUCCESS;
	}

	// Load the image, landmarks, LandmarkMapper and the Morphable Model:
	Mat image = cv::imread(imagefile.string());
    std::vector<cv::Vec2f> landmarks;
	try {
		landmarks = read_pts_landmarks(landmarksfile.string());
	}
	catch (const std::runtime_error& e) {
		cout << "Error reading the landmarks: " << e.what() << endl;
		return EXIT_FAILURE;
	}

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief eos_run_test
    /// 获取模型
    eos_run eos_run_test;
    eos_run_test.run(modelfile, mappingsfile, image, landmarks, outputfilename, outputfilepath);

	return EXIT_SUCCESS;
}
