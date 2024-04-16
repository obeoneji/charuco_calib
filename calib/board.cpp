#include "Board.h"

#include <iostream>

using std::vector;
using namespace cv;

namespace {

    /**
     * Spilt a pre-defined ArUco pattern dictionary into smaller ones 
     * according to the offset size.
     * 
     * \param dict the dictionary to be splitted
     * \param offset the offset size of the splitted dicts
     * \return an array of all splitted dictionaries with smaller sizes
     */
    vector<ArUcoDictionaryPtr> splitDictionary(ArUcoDictionaryPtr dict, int offset, int max_size = 1024) {
        vector<ArUcoDictionaryPtr> splits;
        int count = 0;
 
        while (count < max_size && dict->bytesList.rows >= offset) {
            Mat splitBytesList;
            Rect current(0, 0, dict->bytesList.cols, offset);
            Rect remaining(0, offset, dict->bytesList.cols, dict->bytesList.rows - offset);
            dict->bytesList(current).copyTo(splitBytesList);
            dict->bytesList(remaining).copyTo(dict->bytesList);

            splits.push_back(std::make_shared<aruco::Dictionary>(splitBytesList, dict->markerSize, dict->maxCorrectionBits));
            ++count;
        }

        return splits;
    }
}

/**
 * Constructor with charuco parameters.
 * 
 * \param board_params 
 */
ChArUcoBoardBundle::ChArUcoBoardBundle(const ChArUcoParams& _params) :
    params(_params)
{
    ArUcoDictionaryPtr pred = aruco::getPredefinedDictionary(params.predifined_dict);
    dicts = splitDictionary(pred, params.split_offset, params.max_board_num);

    for (ArUcoDictionaryPtr dict : dicts) {
        auto board = aruco::CharucoBoard::create(params.cols, params.rows, params.square_size, params.marker_size, dict);
        boards.push_back(board);
    }

    bundle_size = dicts.size();
}


/**
 * Draw all boards into opencv images.
 * 
 * \return 
 */
vector<Mat> ChArUcoBoardBundle::drawAllBoards()
{
    vector<Mat> board_images;

    for (ChArUcoBoardPtr board : boards) {
        Mat img;
        auto img_size = Size(params.cols * params.square_size_pixel, params.rows * params.square_size_pixel);
        board->draw(img_size, img, params.margin_width_pixel, params.marker_border_bit_size);
        board_images.push_back(std::move(img));
    }
    
    return board_images;
}
