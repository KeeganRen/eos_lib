/*
 * Eos - A 3D Morphable Model fitting library written in modern C++11/14.
 *
 * File: include/eos/render/utils.hpp
 *
 * Copyright 2014, 2015 Patrik Huber
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <eos/render/utils.h>

namespace eos {
	namespace render {

/**
 * Calculates the normal of a face (or triangle), i.e. the
 * per-face normal. Return normal will be normalised.
 * Assumes the triangle is given in CCW order, i.e. vertices
 * in counterclockwise order on the screen are front-facing.
 *
 * @param[in] v0 First vertex.
 * @param[in] v1 Second vertex.
 * @param[in] v2 Third vertex.
 * @return The unit-length normal of the given triangle.
 */
cv::Vec3f calculate_face_normal(const cv::Vec3f& v0, const cv::Vec3f& v1, const cv::Vec3f& v2)
{
	cv::Vec3f n = (v1 - v0).cross(v2 - v0); // v0-to-v1 x v0-to-v2
	n /= cv::norm(n);
	return n;
};

/**
 * Draws the texture coordinates (uv-coords) of the given mesh
 * into an image by looping over the triangles and drawing each
 * triangle's texcoords.
 *
 * @param[in] mesh A mesh with texture coordinates.
 * @param[in] image An optional image to draw onto.
 * @return An image with the texture coordinate triangles drawn in it, 512x512 if no image is given.
 */
cv::Mat draw_texcoords(Mesh mesh, cv::Mat image = cv::Mat())
{
	using cv::Point2f;
	using cv::Scalar;
	if (image.empty())
	{
		image = cv::Mat(512, 512, CV_8UC4, Scalar(0.0f, 0.0f, 0.0f, 255.0f));
	}

	for (const auto& triIdx : mesh.tvi) {
		cv::line(image, Point2f(mesh.texcoords[triIdx[0]][0] * image.cols, mesh.texcoords[triIdx[0]][1] * image.rows), Point2f(mesh.texcoords[triIdx[1]][0] * image.cols, mesh.texcoords[triIdx[1]][1] * image.rows), Scalar(255.0f, 0.0f, 0.0f));
		cv::line(image, Point2f(mesh.texcoords[triIdx[1]][0] * image.cols, mesh.texcoords[triIdx[1]][1] * image.rows), Point2f(mesh.texcoords[triIdx[2]][0] * image.cols, mesh.texcoords[triIdx[2]][1] * image.rows), Scalar(255.0f, 0.0f, 0.0f));
		cv::line(image, Point2f(mesh.texcoords[triIdx[2]][0] * image.cols, mesh.texcoords[triIdx[2]][1] * image.rows), Point2f(mesh.texcoords[triIdx[0]][0] * image.cols, mesh.texcoords[triIdx[0]][1] * image.rows), Scalar(255.0f, 0.0f, 0.0f));
	}
	return image;
};

// TODO: Should go to detail:: namespace, or texturing/utils or whatever.
unsigned int get_max_possible_mipmaps_num(unsigned int width, unsigned int height)
{
	unsigned int mipmapsNum = 1;
	unsigned int size = std::max(width, height);

	if (size == 1)
		return 1;

	do {
		size >>= 1;
		mipmapsNum++;
	} while (size != 1);

	return mipmapsNum;
};

// throws: ocv exc,  runtime_ex
Texture create_mipmapped_texture(cv::Mat image, unsigned int mipmapsNum = 0)
{
	assert(image.type() == CV_8UC3 || image.type() == CV_8UC4);

	Texture texture;

	texture.mipmaps_num = (mipmapsNum == 0 ? get_max_possible_mipmaps_num(image.cols, image.rows) : mipmapsNum);
	/*if (mipmapsNum == 0)
	{
	uchar mmn = render::utils::MatrixUtils::getMaxPossibleMipmapsNum(image.cols, image.rows);
	this->mipmapsNum = mmn;
	} else
	{
	this->mipmapsNum = mipmapsNum;
	}*/

	if (texture.mipmaps_num > 1)
	{
		if (!is_power_of_two(image.cols) || !is_power_of_two(image.rows))
		{
			throw std::runtime_error("Error: Couldn't generate mipmaps, width or height not power of two.");
		}
	}
	if (image.type() == CV_8UC3)
	{
		image.convertTo(image, CV_8UC4); // Most often, the input img is CV_8UC3. Img is BGR. Add an alpha channel
		cv::cvtColor(image, image, CV_BGR2BGRA);
	}

	int currWidth = image.cols;
	int currHeight = image.rows;
	std::vector<cv::Mat> mipmaps;
	for (int i = 0; i < texture.mipmaps_num; i++)
	{
		if (i == 0) {
			mipmaps.push_back(image);
		}
		else {
			cv::Mat currMipMap(currHeight, currWidth, CV_8UC4);
			cv::resize(mipmaps[i - 1], currMipMap, currMipMap.size());
			mipmaps.push_back(currMipMap);
		}

		if (currWidth > 1)
			currWidth >>= 1;
		if (currHeight > 1)
			currHeight >>= 1;
	}
	texture.mipmaps = mipmaps;
	texture.widthLog = (uchar)(std::log(mipmaps[0].cols) / CV_LOG2 + 0.0001f); // std::epsilon or something? or why 0.0001f here?
	texture.heightLog = (uchar)(std::log(mipmaps[0].rows) / CV_LOG2 + 0.0001f); // Changed std::logf to std::log because it doesnt compile in linux (gcc 4.8). CHECK THAT
	return texture;
};

	} /* namespace render */
} /* namespace eos */
