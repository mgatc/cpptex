#ifndef CPPTEX_GRAPHPRINTER_H
#define CPPTEX_GRAPHPRINTER_H

#include <iomanip>
#include <sstream>
#include <string>
#include <utility> // pair

#include "./cpptex/detail/TikzPrinter.h"
#include "./cpptex/detail/util.h"

namespace cpptex {

class GraphPrinter : public TikzPrinter {
public:
    std::string activeEdgeColor =     "000000";
    std::string inactiveEdgeColor =   "bbbbbb";
    std::string triangulationEdgeColor =   "dddddd";
    std::string worstPathEdgeColor =  "ff0000";
    std::string activeVertexColor =   "0000ff";
    std::string inactiveVertexColor = "f7b267";
    std::string backgroundColor =     "FEFEF6";
    std::string textColor =           "111116";
    double vertexRadius = 0.05;
    double activeEdgeWidth =0.6;
    double inactiveEdgeWidth = 0.4;


    OptionsList activeEdgeOptions = { // active edge options
            {"color",      activeEdgeColor},
            {"line width", std::to_string(activeEdgeWidth)}
    };
    OptionsList inactiveEdgeOptions = { // active edge options
            {"color",      inactiveEdgeColor},
            {"line width", std::to_string(inactiveEdgeWidth)}
    };
    OptionsList triangulationEdgeOptions = { // active edge options
            {"densely dashed", ""},
            {"color",      inactiveEdgeColor},
            {"line width", std::to_string(inactiveEdgeWidth/2)}
    };
    OptionsList coneOptions = { // active edge options
            {"color",      activeEdgeColor},
            {"line width", std::to_string(inactiveEdgeWidth/3)},
            {"densely dotted",  ""}
    };
    OptionsList highlightEdgeOptions = { // active edge options
            //{"densely dashed", ""},
            {"color",          worstPathEdgeColor},
            {"line width",     std::to_string(activeEdgeWidth)}
    };
    OptionsList highlightVertexOptions = {
            {"diamond",    ""},
            {"vertex",     (std::to_string(vertexRadius * 1.61))}, // vertex width
            {"color",      (worstPathEdgeColor)}, // text color
            {"fill",       (worstPathEdgeColor)}, // vertex color
            {"line width", (std::to_string(0))} // vertex border (same color as text)
    };
    OptionsList activeVertexOptions = {
            {"circle",     ""},
            {"vertex",     (std::to_string(vertexRadius))}, // vertex width
            {"color",      (backgroundColor)}, // text color
            {"fill",       (activeVertexColor)}, // vertex color
            {"line width", (std::to_string(0))} // vertex border (same color as text)
    };
    OptionsList borderOptions = {
            {"border",     (std::to_string(vertexRadius))}, // choose shape of vertex
            {"color",      activeEdgeColor}, // additional border color
            {"line width", std::to_string(inactiveEdgeWidth)}, // additional border width
    };

    explicit GraphPrinter(std::string directory, std::string filename, std::string documentType = "standalone")
            : TikzPrinter(directory,filename,documentType){

        // setup graph environment
        std::string tikzOptions = getTikzOptions();
        //cout<<tikzOptions<<endl;
        std::string tikzHeader = getTikzHeader(tikzOptions);
        //cout<<tikzHeader<<endl;
        m_body = Body{ tikzHeader,
                     // getTikzGrid() +
                      "\n\n",
                      getTikzFooter()};

        // define colors
        defineColor(activeEdgeColor);
        defineColor(inactiveEdgeColor);
        defineColor(worstPathEdgeColor);
        defineColor(triangulationEdgeColor);
        defineColor(activeVertexColor);
        defineColor(inactiveVertexColor);
        defineColor(backgroundColor);
    }
    std::string getTikzOptions() {
        return std::string("vertex/.style = {circle,fill, minimum size=")
               + std::to_string(vertexRadius)
               + "cm, inner sep=0pt, outer sep=0pt}, "
               + "vertex/.default = 6pt, font=\\tiny";
    }

    template< typename RandomAccessIterator, typename PointContainer >
    void drawEdges( RandomAccessIterator edgesBegin, RandomAccessIterator edgesEnd, const PointContainer &P, const OptionsList& options = {} ) {
        for( auto e=edgesBegin; e!=edgesEnd; ++e ) {
            drawEdge(P[e->first], P[e->second], options);
        }
    }
    template< typename P>
    void drawEdge(const P& lhs, const P& rhs, const OptionsList& options = {} ) {
        drawLine( lhs.x(),
                  lhs.y(),
                  rhs.x(),
                  rhs.y(), options );
    }

    template< typename Triangulation >
    void drawEdges( const Triangulation& T, const OptionsList& options = {} ) {
        for( auto eit = T.finite_edges_begin(); eit != T.finite_edges_end(); ++eit ) {
            auto e = *eit;
            double x1 = e.first->vertex( (e.second+1)%3 )->point().x();
            double y1 = e.first->vertex( (e.second+1)%3 )->point().y();
            double x2 = e.first->vertex( (e.second+2)%3 )->point().x();
            double y2 = e.first->vertex( (e.second+2)%3 )->point().y();
            drawLine( x1,y1,x2,y2,options );
        }
        m_body.content += "\n";
    }

    template< typename Triangulation >
    void drawEdgesOfSDG( const Triangulation& T, const OptionsList& options = {} ) {
        for( auto eit = T.finite_edges_begin(); eit != T.finite_edges_end(); ++eit ) {
            auto e = *eit;
            double x1 = e.first->vertex( (e.second+1)%3 )->site().point().x();
            double y1 = e.first->vertex( (e.second+1)%3 )->site().point().y();
            double x2 = e.first->vertex( (e.second+2)%3 )->site().point().x();
            double y2 = e.first->vertex( (e.second+2)%3 )->site().point().y();
            drawLine( x1,y1,x2,y2,options );
        }
        m_body.content += "\n";
    }


    template< typename T >
    void drawVertices( const T &Triangulation, const OptionsList& options = {}, const OptionsList& borderOptions = {} ) {
        for( typename T::Finite_vertices_iterator it = Triangulation.finite_vertices_begin(); it != Triangulation.finite_vertices_end(); ++it )
            drawVertexWithLabel( it->point().x(), it->point().y(), "", options, borderOptions );
        m_body.content += "\n";
    }

    template< typename InputIterator >
    void drawVertices( const InputIterator &pointsStart, const InputIterator &pointsEnd, const OptionsList& options = {}, const OptionsList& borderOptions = {} ) {
        size_t id = 0;
        for( auto it=pointsStart; it!=pointsEnd; ++it )
            drawVertexWithLabel( it->x(), it->y(), "", options, borderOptions );
        m_body.content += "\n";
    }

    template< typename T >
    void drawVerticesWithInfo( const T &Triangulation, const OptionsList& options = {}, const OptionsList& borderOptions = {} ) {
        for( typename T::Finite_vertices_iterator it = Triangulation.finite_vertices_begin(); it != Triangulation.finite_vertices_end(); ++it )
            drawVertexWithLabel( it->point().x(), it->point().y(), to_string(it->info()), options, borderOptions );
        m_body.content += "\n";
    }

    template< typename InputIterator >
    void drawVerticesWithInfo( const InputIterator &pointsStart, const InputIterator &pointsEnd, const OptionsList& options = {}, const OptionsList& borderOptions = {} ) {
        size_t id = 0;
        for( auto it=pointsStart; it!=pointsEnd; ++it )
            drawVertexWithLabel( it->x(), it->y(), std::to_string(id++), options, borderOptions );
        m_body.content += "\n";
    }

    template< typename T >
    void drawVerticesWithInfoSDG( const T &Triangulation, const OptionsList& options = {}, const OptionsList& borderOptions = {} ) {
        for( typename T::Finite_vertices_iterator it = Triangulation.finite_vertices_begin(); it != Triangulation.finite_vertices_end(); ++it )
            drawVertexWithLabel( it->site().point().x(), it->site().point().y(), to_string(it->storage_site().info()), options, borderOptions );
        m_body.content += "\n";
    }

    template< typename T >
    void drawVertexPair( const std::pair<typename T::Vertex_handle,typename T::Vertex_handle>& vertices, const OptionsList& options = {} ) {
        drawVertex( vertices.first->point().x(), vertices.first->point().y(), options );
        drawVertex( vertices.second->point().x(), vertices.second->point().y(), options );
    }

    void drawVertex( double x, double y, const OptionsList& options = {} ) {
        drawVertexWithLabel( x, y, "", options );
    }

    void drawVertexWithLabel( double x, double y, const std::string &label, const OptionsList& options = {}, const OptionsList& borderOptions = {} ) {

        if( label.empty() ){
            std::stringstream stream;
            stream << std::fixed << std::setprecision(2);
            stream << x << "," << y;
            //label = stream.str();
        }
        m_body.content += "\\node (vertex" + label + ") [fill,"
                          + expandOptions( options )
                          + "] at ("
                          + std::to_string( x*_scaleFactor ) + ","
                          + std::to_string( y*_scaleFactor )
                          + ") {"
                          + label
                          + "};\n";
    }
//
//    void drawEdges( const spanner::DelaunayGraph& DG, const OptionsList& options = {} ) {
//        for( const auto &el : DG.m_E ) {
//            for( const auto& v : el.second ) {
//                //std::cout << el.first->point() << " " << v->point() << "\n";
//                double x1 = el.first->point().x();
//                double y1 = el.first->point().y();
//                double x2 = v->point().x();
//                double y2 = v->point().y();
//                drawLine( x1,y1,x2,y2,options );
//            }
//        }
//        m_body.content += "\n";
//    }

//    template< typename Triangulation >
//    void drawEdgesOfHalfTheta( const Triangulation& T, const OptionsList& options = {} ) {
////        using CGAL::to_double;
//        //typedef typename Triangulation::VertexHandle VertexHandle;
//        for( auto eit = T.edges_begin(); eit != T.edges_end(); ++eit ) {
//            auto e = *eit;
//
//            typename Triangulation::Point_2
//                    vp = T._G[source(e,T._G)],
//                    vq = T._G[target(e,T._G)];
//
//            double x1 = to_double(vp.x());
//            double y1 = to_double(vp.y());
//            double x2 = to_double(vq.x());
//            double y2 = to_double(vq.y());
//            drawLine( x1,y1,x2,y2,options );
//        }
//        m_body.content += "\n";
//    }

    template< typename P>
    void drawCones(const P& center, const P& orientation, size_t numCones, double length, const OptionsList& options = {} ) {
        double xCenter = center.x(),
               yCenter = center.y(),
               xTranslated = orientation.x() - xCenter,
               yTranslated = orientation.y() - yCenter,
               orientationLength = sqrt(pow(xTranslated,2) + pow(yTranslated,2)),
               sizingFactor = length / orientationLength,
               x = xTranslated*sizingFactor + xCenter,
               y = yTranslated*sizingFactor + yCenter,
               theta = cpptex::PI / numCones;

        for( size_t i=0; i<numCones; ++i) {
            drawLine(x,y,xCenter,yCenter,options);
            xTranslated = x - xCenter;
            yTranslated = y - yCenter;
            x = xCenter + xTranslated*cos(theta) + yTranslated*sin(theta);
            y = yCenter - xTranslated*sin(theta) + yTranslated*cos(theta);
        }
    }

    void drawLine( double x1, double y1, double x2, double y2, const OptionsList& options = {} ) {
        m_body.content += "\\draw [" + expandOptions(options ) + "] ("
                          + std::to_string(x1*_scaleFactor) + "," + std::to_string(y1*_scaleFactor) + ") -- ("
                          + std::to_string(x2*_scaleFactor) + "," + std::to_string(y2*_scaleFactor) + ");\n";
    }
    std::string getTikzGrid() const {
        return "\\draw[step=1.0,black,thin,dotted] (-5.5,-5.5) grid (5.5,5.5);";
    }
private:
    double m_autoscaleVertexSizeFactor = 0.02;
}; // class GraphPrinter

} // namespace cpptex

#endif // CPPTEX_GRAPHPRINTER_H
