#include "eos_lib/eos_run.h"

#include "eos/core/Landmark.h"
#include "eos/core/LandmarkMapper.h"
#include "eos/fitting/nonlinear_camera_estimation.h"
#include "eos/fitting/linear_shape_fitting.h"
#include "eos/render/utils.h"
#include "eos/render/texture_extraction.h"

using namespace eos;
using eos::core::Landmark;
using eos::core::LandmarkCollection;

eos_run::eos_run()
{
}

int eos_run::run(boost::filesystem::path &modelfile, boost::filesystem::path &mappingsfile,
                 cv::Mat &image, vector<cv::Vec2f> &landmarks_std,
                 fs::path &outputfilename, fs::path &outputfilepath)
{
    LandmarkCollection<Vec2f> landmarks;
    landmarks.reserve(68);
    for (size_t i = 0; i < landmarks_std.size(); ++i)
    {
        cv::Vec2f vec2f_landmark = landmarks_std.at(i);
        Landmark<cv::Vec2f> landmark;
        landmark.name = std::to_string(i+1);
        landmark.coordinates[0] = vec2f_landmark[0];
        landmark.coordinates[1] = vec2f_landmark[1];
        landmark.coordinates[0] -= 1.0f;
        landmark.coordinates[1] -= 1.0f;
        landmarks.emplace_back(landmark);
    }

    morphablemodel::MorphableModel morphable_model;
    try {
        morphable_model = morphablemodel::load_model(modelfile.string());
    }
    catch (const std::runtime_error& e) {
        cout << "Error loading the Morphable Model: " << e.what() << endl;
        return EXIT_FAILURE;
    }
    core::LandmarkMapper landmark_mapper = mappingsfile.empty() ? core::LandmarkMapper() : core::LandmarkMapper(mappingsfile);

    // Draw the loaded landmarks:
    Mat outimg = image.clone();
    int i = 1;
    for (auto&& lm : landmarks) {
        cv::Point numPoint(lm.coordinates[0] - 2.0f, lm.coordinates[1] - 2.0f);
        cv::rectangle(outimg, cv::Point2f(lm.coordinates[0] - 2.0f, lm.coordinates[1] - 2.0f), cv::Point2f(lm.coordinates[0] + 2.0f, lm.coordinates[1] + 2.0f), { 255, 0, 0 });
        /// Keegan.Ren
        /// TODO: plot the face point and point number in the image
        char str_i[11];
        sprintf(str_i,"%d",i);
        cv::putText(outimg, str_i, numPoint, CV_FONT_HERSHEY_COMPLEX, 0.3, cv::Scalar(0,0,255));
        ++i;
    }
    cout << "i = " << i << endl;
    cv::imshow("rect_outimg", outimg);
    cv::waitKey();

    // These will be the final 2D and 3D points used for the fitting:
    vector<Vec4f> model_points; // the points in the 3D shape model
    vector<int> vertex_indices; // their vertex indices
    vector<Vec2f> image_points; // the corresponding 2D landmark points

    // Sub-select all the landmarks which we have a mapping for (i.e. that are defined in the 3DMM):
    for (int i = 0; i < landmarks.size(); ++i) {
        auto converted_name = landmark_mapper.convert(landmarks[i].name);
        if (!converted_name) { // no mapping defined for the current landmark
            continue;
        }
        int vertex_idx = std::stoi(converted_name.get());
        Vec4f vertex = morphable_model.get_shape_model().get_mean_at_point(vertex_idx);
        model_points.emplace_back(vertex);
        vertex_indices.emplace_back(vertex_idx);
        image_points.emplace_back(landmarks[i].coordinates);
    }

    /// Keegan.Ren
    cout << "model_point = " << endl;
    for (int i = 0; i < landmarks.size(); ++i) {
        //         3d points                                                    2d points
        cout << model_points[i] << "\t" << vertex_indices[i] << "\t" << image_points[i] << endl;
    }


    // Estimate the camera (pose) from the 2D - 3D point correspondences
    fitting::OrthographicRenderingParameters rendering_params = fitting::estimate_orthographic_camera(image_points, model_points, image.cols, image.rows);
    /// Keegan.Ren
    cout << "rendering_params = " << endl;
    std::cout << rendering_params.r_x << " " << rendering_params.r_y << " " << rendering_params.r_z << " " << rendering_params.t_x << " " << rendering_params.t_y << endl;
    std::cout << rendering_params.frustum.b << " " << rendering_params.frustum.l << " " << rendering_params.frustum.r << " " << rendering_params.frustum.t << endl;
    Mat affine_from_ortho = get_3x4_affine_camera_matrix(rendering_params, image.cols, image.rows);
    // Keegan
    cout << "affine_from_ortho = " << endl;
    cout << affine_from_ortho << endl;
    // 	cv::imshow("affine_from_ortho", affine_from_ortho);
    // 	cv::waitKey();

    // The 3D head pose can be recovered as follows:
    float xaw_angle = glm::degrees(rendering_params.r_x);
    float yaw_angle = glm::degrees(rendering_params.r_y);
    float zaw_angle = glm::degrees(rendering_params.r_z);
    cout << "x_y_z_angle = " << endl;
    cout << xaw_angle << "\t" << yaw_angle << "\t" << zaw_angle << endl;
    // and similarly for pitch (r_x) and roll (r_z).

    // Estimate the shape coefficients by fitting the shape to the landmarks:
    //                                                                    bin模型           投影矩阵              图片二维点
    vector<float> fitted_coeffs = fitting::fit_shape_to_landmarks_linear(morphable_model, affine_from_ortho, image_points, vertex_indices);
    cout << "size = " << fitted_coeffs.size() << endl;
    for (int i = 0; i < fitted_coeffs.size(); ++i)
        cout << fitted_coeffs[i] << endl;

    // Obtain the full mesh with the estimated coefficients:
    render::Mesh mesh = morphable_model.draw_sample(fitted_coeffs, vector<float>());

    // Extract the texture from the image using given mesh and camera parameters:
    cv::Mat isomap = render::extract_texture(mesh, affine_from_ortho, image);

//    // Save the mesh as textured obj:
//    outputfile += fs::path(".obj");
//    render::write_textured_obj(mesh, outputfilename.string(), outputfilepath.string());

    std::cout << outputfilename << std::endl;
    render::write_textured_obj(mesh, outputfilename.string(), outputfilepath.string());

    // And save the isomap:
    outputfilename.replace_extension(".isomap.png");
    cv::imwrite(outputfilepath.string() + outputfilename.string(), isomap);

    //    cv::imshow(outputfile.string(), isomap);
    //    cv::waitKey();

    cout << "Finished fitting and wrote result mesh and isomap to files with basename " << outputfilename.stem().stem() << "." << endl;

    return true;
}
