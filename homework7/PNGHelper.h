#ifndef PNG_IMAGE_HELPER_H
#define PNG_IMAGE_HELPER_H

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

#include <iostream>
#include <vector>
#include <algorithm>
#include "PNG.h"

// Use PixBuf as a shortcut datatype to refer to
// std::std::vector<unsigned char>
typedef std::vector<unsigned char> PixBuf;

/**
   A simple class to wrap a PixBuf::iterator and access 3 consecutive
   values as Red, Green, & Blue components of a RGB pixel.
*/
class Pixel {
    friend std::ostream& operator<<(std::ostream& os, const Pixel& pix) {
        return os << "(" << static_cast<int>(*(pix.start + 0))
                  << "," << static_cast<int>(*(pix.start + 1))
                  << "," << static_cast<int>(*(pix.start + 2))
                  << ")";
    }    

public:
    explicit inline Pixel(PixBuf::iterator start) : start(start) {}
    inline ~Pixel() {}

    static int max(const int dRed, const int dGreen, const int dBlue) {
        return std::max(dRed, std::max(dGreen, dBlue));
    }
    
    inline int operator-(const Pixel& other) const {
        return max(std::abs(*(start + 0) - *(other.start + 0)),
                   std::abs(*(start + 1) - *(other.start + 1)),
                   std::abs(*(start + 2) - *(other.start + 2)));
    }
    
    inline Pixel& operator=(const Pixel& src) {
        *(start + 0) = *(src.start + 0);
        *(start + 1) = *(src.start + 1);
        *(start + 2) = *(src.start + 2);
        *(start + 3) = *(src.start + 3);
        return *this;
    }
    
    inline void addTo(int& red, int& green, int& blue) {
        red   += *(start + 0);
        green += *(start + 1);
        blue  += *(start + 2);
    }
    
    inline bool operator==(const Pixel& other) const {
        return (*(start + 0) == *(other.start + 0) &&
                *(start + 1) == *(other.start + 1) &&
                *(start + 2) == *(other.start + 2));
    }
    
private:
    // The pixel buffer that holds information about the RGB data for
    // this pixel. This vector is essentially reference to a
    // sub-vector within the image associated with this pixel.
    PixBuf::iterator start;
};

/**
   A simple wrapper class around PNG that provides convenient access
   (via a functor) to a sub-image starting at a given row and column
   position.
*/
class PNGHelper {
public:
    explicit PNGHelper(PNG& png, int startRow = 0, int startCol = 0) :
        startRow(startRow), startCol(startCol), png(png) {}
    
    virtual ~PNGHelper() {}
    
    inline Pixel operator()(int row, int col) const {
        const int offset = ((startRow + row) * png.getWidth() +
                            (startCol + col)) * 4;
        return Pixel(png.getBuffer().begin() + offset);
    }

    /** Returns the widht of this PNG image.

        \return The width of this PNG image.
    */
    inline int getWidth() const { return png.getWidth(); }

    /** Returns the height of this PNG image.

        \return The height of this PNG image.
    */    
    inline int getHeight() const { return png.getHeight(); }

    /** Returns the number of pixels in this PNG image.

        \return The number of pixels in this PNG image determined as
        width * height.
    */    
    inline int getPixCount() const { return getWidth() * getHeight(); }
    
protected:
    int startRow;
    int startCol;
    
private:
    PNG& png;
};

#endif