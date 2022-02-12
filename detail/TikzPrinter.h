#ifndef CPPTEX_TIKZPRINTER_H
#define CPPTEX_TIKZPRINTER_H

#include <algorithm>
#include <cmath>
#include <string>

#include "LatexPrinter.h"

namespace cpptex {

class TikzPrinter : public LatexPrinter {
  public:

    explicit TikzPrinter(std::string directory, std::string filename, std::string documentType = "standalone")
        : LatexPrinter(directory,filename,documentType){

        // setup graph environment
        //string tikzOptions = getTikzOptions();
        m_body = Body{getTikzHeader(), "", getTikzFooter()};
    }

    //TODO: this version of autoscale should be a member of GraphPrinter since it requires points
    // begin and end are iterators over the point set that will be printed
    // this is for proper scaling of the points to work with the latex document
    // the points will not be printed unless you call drawVertices() on them
    template< class InputIterator>
    void autoscale( InputIterator pointsBegin, InputIterator pointsEnd, double documentSize = 10 )
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

        double deltaX = std::abs( minX - maxX ),
               deltaY = std::abs( minY - maxY ),
               delta  = std::max( deltaX, deltaY );

        _scaleFactor = documentSize / delta;
        //vertexRadius = documentSize * m_autoscaleVertexSizeFactor;
        //m_body.header = getTikzHeader();//getTikzOptions());
    }

    // Tikz getters
    std::string getTikzHeader(std::string options = "") const {
        std::string header = "\\begin{tikzpicture}";
        if(!options.empty()){
            header += "[" + options + "]";
        }
        header += "\n\n";
        return header;
    }
    std::string getTikzFooter() const {
        return "\\end{tikzpicture}\n\n";
    }
protected:
    double _scaleFactor = 1;
    double _resizeFactor = 1;
}; // class TikzPrinter

} // namespace cpptex

#endif // CPPTEX_TIKZPRINTER_H
