#ifndef CPPTEX_TIKZPRINTER_H
#define CPPTEX_TIKZPRINTER_H

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>

#include "LatexPrinter.h"

namespace cpptex {

struct Point {
    double x = std::numeric_limits<double>::infinity();
    double y = std::numeric_limits<double>::infinity();
};

class TikzPrinter : public LatexPrinter {
  public:

    explicit TikzPrinter(std::string path, std::string documentType = "standalone")
        : LatexPrinter(path,documentType){

        // setup graph environment
        //string tikzOptions = getTikzOptions();
        m_body = Body{getTikzHeader(), "", getTikzFooter()};
    }

    void autoscale(double minX, double minY, double maxX, double maxY, double sizeInCm) {
        double deltaX = std::abs( minX - maxX ),
                deltaY = std::abs( minY - maxY ),
                delta  = std::max( deltaX, deltaY );
        _scaleFactor = sizeInCm / delta;
    }

    // begin and end are iterators over the point set that will be printed
    // this is for proper scaling of the points to work with the latex document
    // the points will not be printed unless you call drawVertices() on them
    template< class InputIterator>
    void autoscale( InputIterator pointsBegin, InputIterator pointsEnd, double sizeInCm = 10.0 )
    {
        double minX = pointsBegin->x(),
                maxX = minX,
                minY = pointsBegin->y(),
                maxY = minY;

        for( auto p=pointsBegin; p!=pointsEnd; ++p )
        {
            minX = std::min( p->x(), minX );
            maxX = std::max( p->x(), maxX );
            minY = std::min( p->y(), minY );
            maxY = std::max( p->y(), maxY );
        }
        autoscale(minX,minY, maxX,maxY, sizeInCm);
    }

    // Tikz getters
    static std::string getTikzHeader(std::string options = "") {
        std::string header = "\\begin{tikzpicture}";
        if(!options.empty()){
            header += "[" + options + "]";
        }
        header += "\n\n";
        return header;
    }
    static std::string getTikzFooter() {
        return "\\end{tikzpicture}\n\n";
    }
protected:
    double _scaleFactor = 1;
    double _resizeFactor = 1;
}; // class TikzPrinter

} // namespace cpptex

#endif // CPPTEX_TIKZPRINTER_H
