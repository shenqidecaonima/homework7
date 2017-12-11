#ifndef IMAGE_SEARCH_H
#define IMAGE_SEARCH_H

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

#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include "PNGHelper.h"

/**
   A simple class that maintains the 4-coordinates of matched regions
   of an image. The class also provides a few convenience methods to
   ease operations and interactions with the data in this object.
*/
class MatchedRect {
    // A simple stream insertion operator to print matches.
    friend std::ostream& operator<<(std::ostream& os, const MatchedRect& rect) {
        return os << "sub-image matched at: " << rect.row1 << ", " 
                  << rect.col1 << ", " << rect.row2 << ", " << rect.col2;
    }

public:
    MatchedRect() { row1 = col1 = row2 = col2 = 0; }
    MatchedRect(int row, int col, int width, int height) :
        row1(row), col1(col), row2(row + height), col2(col + width) {}
    
    inline bool contains(const int row, const int col) const {
        return ((row >= row1) && (row < row2) && (col >= col1) && (col < col2));
    }
    
    inline bool intersects(const MatchedRect& other) const {
        return !((row1 > other.row2) || (row2 < other.row1) ||
                 (col1 > other.col2) || (col2 < other.col1));
    }

    inline bool operator()(const MatchedRect& other) const {
        return intersects(other);
    }

    inline bool operator<(const MatchedRect& other) const {
        return ((row1 == other.row1) ? (col1 < other.col1) :
                (row1 < other.row1));
    }
    
    // The four corners of the matched region.
    int row1, col1, row2, col2;
};

/**
   A convenience wrapper class around std::vector to encapsulate the
   list of matched regions in the image. 
*/
class MatchedRectList : public std::vector<MatchedRect> {
    friend std::ostream& operator<<(std::ostream& os,
                                    const MatchedRectList& list) {
        std::copy(list.begin(), list.end(),
                  std::ostream_iterator<MatchedRect>(os, "\n"));
        return os;
    }

public:
    MatchedRectList() {}
    MatchedRectList(const MatchedRectList& rhs) :
        std::vector<MatchedRect>(rhs) {}    
    ~MatchedRectList() {}

    MatchedRectList& operator+=(const MatchedRectList& rhs) {
        for (const MatchedRect& other : rhs) {
            (*this) += other;
        }
        return *this;
    }
    
    inline bool isMatched(const MatchedRect& other) const {
        bool isMatched = false;
        isMatched = find_if(begin(), end(), other) != end();
        return isMatched;
    }
    
    MatchedRectList& operator+=(const MatchedRect& rect) {
        if (find_if(begin(), end(), rect) == end()) {
            push_back(rect);
        }
        return *this;
    }
};


class ImageSearch {
public:
    ImageSearch(const std::string& mainImageFile,
                const std::string& srchImageFile,
                const bool isMask, const int matchPercent,
                const int tolerance);
    
    ~ImageSearch() {}

    void search();
    
    void generateResults(const std::string& resultImageFile);
    
protected:
    void search(const PNGHelper& mask, const int row, const int col,
                MatchedRectList& mrl);
    void search(PNG& srchImg);
    int getMatchingPixelCount(const bool isMask, const PNGHelper& img1,
                              const PNGHelper& img2,
                              const int maxRow, const int maxCol,
                              const int tolerance) const;
    
    int getImgPixelCount(const PNGHelper& img1,
                         const PNGHelper& img2,
                         const int maxRow, const int maxCol,
                         const int tolerance) const;
    
    int getMaskPixelCount(const PNGHelper& img1,
                          const PNGHelper& img2,
                          const int maxRow, const int maxCol,
                          const int tolerance) const;    
    
    void computeBackgroundPixel(const PNGHelper& subImg, const PNGHelper& mask,
                                const int MaxRow, const int MaxCol,
                                std::vector<unsigned char>& bgBuf) const;
    
    void drawRedBox(PNGHelper& subImg, const MatchedRect& box);
    
    PNG rotate90(PNG& png);
    PNG flipVertical(PNG& png);
    
private:
    MatchedRectList mrl;    
    PNG mainImage;
    PNG subImage;
    const bool isMask;
    const int matchPercent;
    const int tolerance;
};

#endif
