#ifndef IMAGE_SEARCH_CPP
#define IMAGE_SEARCH_CPP

//--------------------------------------------------------------------
//
// Copyright (C) 2015 raodm@miamiOH.edu
//
// Miami University makes no representations or warranties about the
// suitability of the software, either express or implied, including
// but not limited to the implied warranties of merchantability,
// fitness for a particular purpose, or non-infringement.  Miami
// University shall not be liable for any damages suffered by licensee
// as a result of using, result of using, modifying or distributing
// this software or its derivatives.
//
// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of GNU General Public License (version 3).
//
// Authors:   Dhananjai M. Rao          raodm@muohio.edu
//
//---------------------------------------------------------------------

#include <omp.h>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
#include "ImageSearch.h"


ImageSearch::ImageSearch(const std::string& mainImageFile,
                         const std::string& srchImageFile, const bool isMask,
                         const int matchPercent, const int tolerance) :
    isMask(isMask), matchPercent(matchPercent), tolerance(tolerance) {
    mainImage.load(mainImageFile);
    subImage.load(srchImageFile);
}

int
ImageSearch::getImgPixelCount(const PNGHelper& img1, const PNGHelper& img2,
                              const int MaxRow, const int MaxCol,
                              const int tolerance) const {
    int matchingPixelCount = 0;
    for (int row = 0; (row < MaxRow); row++) {
        for (int col = 0; (col < MaxCol); col++) {
            if (img1(row, col) - img2(row, col) < tolerance) {
                matchingPixelCount++;
            }
        }
    }
    return matchingPixelCount;
}

void
ImageSearch::computeBackgroundPixel(const PNGHelper& subImg,
                                    const PNGHelper& mask,
                                    const int maxRow, const int maxCol,
                                    std::vector<unsigned char>& bgBuf) const {
    std::vector<unsigned char> blackBuf = {0, 0, 0, 255};
    const Pixel Black(blackBuf.begin());
    int red = 0, green = 0, blue = 0, count = 0;
    // #pragma omp parallel for reduction(+: red, green, blue, count)
    for (int row = 0; (row < maxRow); row++) {
        for (int col = 0; (col < maxCol); col++) {
            if (mask(row, col) == Black) {
                subImg(row, col).addTo(red, green, blue);
                count++;
            }
        }
    }
    std::cout << "Black pixel count = " << count << std::endl;
    bgBuf = std::vector<unsigned char>{(unsigned char) (red   / count),
                                       (unsigned char) (green / count),
                                       (unsigned char) (blue  / count)};
}
int
ImageSearch::getMaskPixelCount(const PNGHelper& img1, const PNGHelper& mask,
                               const int maxRow, const int maxCol,
                               const int tolerance) const {
    // Compute the average background color using mask information
    std::vector<unsigned char> bgPixColor(4);
    computeBackgroundPixel(img1, mask, maxRow, maxCol, bgPixColor);
    const Pixel BgClr(bgPixColor.begin());
    std::cout << "Background color: " << BgClr << std::endl;
    // A constant pixel to detect black regions in the mask
    std::vector<unsigned char> blackBuf = {0, 0, 0, 255};
    const Pixel Black(blackBuf.begin());
    // The count of matching pixels based on BgClr
    int matchingPixelCount = 0;
    // #pragma omp parallel for reduction(+: matchingPixelCount)
    for (int row = 0; (row < maxRow); row++) {
        for (int col = 0; (col < maxCol); col++) {
            const bool isPixDiff = img1(row, col) - BgClr < tolerance;
            const int  addSub    = (mask(row, col) == Black) ? -1 : 1;
            matchingPixelCount  += addSub * (isPixDiff ? -1 : 1);
        }
    }
    return matchingPixelCount;
}


int
ImageSearch::getMatchingPixelCount(const bool isMask, const PNGHelper& img1,
                                   const PNGHelper& img2,
                                   const int maxRow, const int maxCol,
                                   const int tolerance) const {
    return isMask ? getMaskPixelCount(img1, img2, maxRow, maxCol, tolerance) :
        getImgPixelCount(img1, img2, maxRow, maxCol, tolerance);
}

void
ImageSearch::search(PNG& srchImg, const int row, const int col) {
    const PNGHelper subImageHelper(srchImg);
    // const int maxRow = mainImage.getHeight() - srchImg.getHeight();
    // const int maxCol = mainImage.getWidth()  - srchImg.getWidth();
    // const int pixMatchNeeded = srchImg.getBufferSize() * matchPercent / 400;
    // Search for a match between sub-images.
    PNGHelper mainImgHelper(mainImage, row, col);
    int pixMatch = getMatchingPixelCount(isMask, mainImgHelper,
                                         subImageHelper,
                                         srchImg.getHeight(),
                                         srchImg.getWidth(), tolerance);
    std::cout << "Matching pixels: " << pixMatch << std::endl;
}

void
ImageSearch::search(const int row = 0, const int col = 0) {
    search(subImage, row, col);
/*
    if (isMask) {
        PNG flippedMask = flipVertical(subImage);
        search(flippedMask);
        PNG rotImg = rotate90(subImage);
        search(rotImg);
        rotImg.write("rotated_mask.png");
        rotImg = rotate90(rotImg);
        rotImg = rotate90(rotImg);
        search(rotImg);
    }
*/
}

void
ImageSearch::generateResults(const std::string& resultImageFile) {
    PNG resultImg(mainImage);
    PNGHelper resultHelper(resultImg);
    for (size_t i = 0; (i < mrl.size()); i++) {
        drawRedBox(resultHelper, mrl[i]);
    }
    resultImg.write(resultImageFile);
    std::cout << mrl << "Number of matches: " << mrl.size() << std::endl;
}

PNG
ImageSearch::rotate90(PNG& png) {
    PNG retImg;
    retImg.create(png.getHeight(), png.getWidth());
    PNGHelper ret(retImg), src(png);
    for (int row = 0; (row < png.getHeight()); row++) {
        for (int col = 0; (col < png.getWidth()); col++) {
            ret(col, png.getHeight() - row - 1) = src(row, col);
        }
    }
    return retImg;
}

PNG
ImageSearch::flipVertical(PNG& png) { 
    const int height = png.getHeight(), width = png.getWidth();   
    PNG retImg;
    retImg.create(width, height);
    PNGHelper ret(retImg), src(png);

    for (int row = 0; (row < height); row++) {
        for (int col = 0; (col < width); col++) {
            ret(height - row - 1, col) = src(row, col);
        }
    }
    return retImg;
}

void
ImageSearch::drawRedBox(PNGHelper& subImg, const MatchedRect& box) {
    std::vector<unsigned char> redBuf = {255, 0, 0, 255};
    const Pixel RED(redBuf.begin());
    // Draw horizontal lines for the box.
    for (int col = box.col1; (col < box.col2); col++) {
        subImg(box.row1, col) = RED;
        subImg(box.row2, col) = RED;
    }
    // Draw vertical lines for the box.
    for (int row = box.row1; (row < box.row2); row++) {
        subImg(row, box.col1) = RED;
        subImg(row, box.col2) = RED;
    }    
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        // Insufficient number of required parameters.
        std::cout << "Usage: " << argv[0] << " <MainPNGfile> <SearchPNGfile> "
                  << "<OutputPNGfile> [isMaskFlag] [match-percentage] "
                  << "[tolerance] [row] [col]\n";
        return 1;
    }
    const std::string True("true");
    ImageSearch is(argv[1], argv[2], (argc > 4 ? (True == argv[4]) : false),
                   (argc > 5 ? atoi(argv[5]) : 75),
                   (argc > 6 ? atoi(argv[6]) : 32));
    const int row = (argc > 7 ? atoi(argv[7]) : 0);
    const int col = (argc > 8 ? atoi(argv[8]) : 0);
    is.search(row, col);
    is.generateResults(argv[3]);
    return 0;
}

#endif
