#ifndef CPPTEX_PGFPLOTSPRINTER_H
#define CPPTEX_PGFPLOTSPRINTER_H

#include <cassert>
#include <map>
#include <string>
#include <vector>

#include "TikzPrinter.h"

namespace cpptex {

class PgfplotPrinter : public TikzPrinter {
public:
    typedef std::vector<std::vector<std::pair<double,double>>> ResultMatrix;

    PgfplotPrinter(std::string path, std::string documentType = "standalone")
            : TikzPrinter(path,documentType){
        m_body = Body{getTikzHeader("scale=0.55"),
                      "",
                      getTikzFooter() };
        for(auto color : Colors){
            defineColor(color);
        }
    }
    template<class PlotMap>
    void plotAxis(const std::string& iv, const PlotMap& results, const double xScale = 1.0, const std::string xScaleUnit= "", const bool isFirst = true) {

        assert(abs(xScale) > 0);

//        if(m_algorithmMarkers.empty())
//            for(const auto& spannerName : spanner::bdps::ALGORITHM_NAMES) // provides the ordering we want
//                addNewMarker(spannerName);


        // build axis header
        std::string allPlotsOfAxis = getAxisHeader(iv, results, xScale, xScaleUnit, isFirst);

        // build the plot
//        for( const auto& name : spanner::bdps::ALGORITHM_NAMES ) {
//            if( results.find(name) != results.end() ) {//spanner::contains(results,name) ) {
//                const auto &spanner = results.at(name);
//                std::string ivPlot = getPlotHeader(name);//
//                for (const auto &level: spanner) {
//                    auto xValue = static_cast<double>(level.first) / xScale;
//                    auto yValue = level.second.getIV<double>(iv);
//                    std::string entry = "("
//                                   + std::to_string(xValue)
//                                   + ","
//                                   + std::to_string(yValue)
//                                   + ")\n";
//                    ivPlot += entry;
//                }
//                ivPlot += getPlotFooter();
//                if (isFirst) {
//                    ivPlot += getLegendEntry(name);
//                }
//                allPlotsOfAxis += ivPlot;
//            }
//        }
        allPlotsOfAxis += getAxisFooter(m_filename);
        m_body.content = allPlotsOfAxis;
    }

    void plotAxis(const ResultMatrix& results, const std::vector<std::string>& seriesLabels = {}, std::string xLabel = "", std::string yLabel = "", std::string title = "") {

        if(m_algorithmMarkers.empty())
            for(const auto& seriesName : seriesLabels) // provides the ordering we want
                addNewMarker(seriesName);


        // build axis header
        std::string allPlotsOfAxis = getAxisHeader(results, xLabel, yLabel, title);

        // build the plot
        for(unsigned i=0; i<results.size(); ++i ) {
//            if( results.find(name) != results.end() ) {//spanner::contains(results,name) ) {
//                const auto &spanner = results.at(name);
                std::string ivPlot = getPlotHeader(seriesLabels.size()>i ? seriesLabels[i] : "");//
                for (const auto &level: results[i]) {
                    auto xValue = std::to_string(level.first);
                    auto yValue = std::to_string(level.second);
                    ivPlot += "("
                               + xValue
                               + ","
                               + yValue
                               + ")\n";
//                    ivPlot += entry;
                }
                ivPlot += getPlotFooter();
//                if (isFirst) {
//                    ivPlot += getLegendEntry(name);
//                }
                allPlotsOfAxis += ivPlot;
//            }
        }
        allPlotsOfAxis += getAxisFooter(m_filename);
        m_body.content = allPlotsOfAxis;
    }
    std::string getLegend() {
        std::string refOpener("\\ref{");
        std::string refText = cpptex::removeSpaces(m_caption) + "-legend";
        std::string refCloser("}");

        return refOpener + refText + refCloser;
    }
    std::string getLegendEntry(const std::string& legendText) {
        std::string legendEntry = "\\addlegendentry{\\texttt{"
                             + legendText
                             + "}}";
        return legendEntry;
    }
    std::string getPlotHeader(const std::string& label) {
        std::string plotHeader = "\n\n\\addplot[";

        auto color = getColor();

        plotHeader += std::string("solid,")
                    + getMarkerText(label);

        plotHeader += "]";
        plotHeader += " coordinates {\n";
        return plotHeader;
    }

    std::string getPlotFooter() {
        std::string plotFooter("}node [pos=1.15, above left] {};\n\n");
        return plotFooter;
    }

    std::vector<double> getXTicks(const ResultMatrix& results, unsigned numTicks = 5) {
        if(numTicks == 0) return {};

        double minX = std::numeric_limits<double>::infinity();
        double maxX = -std::numeric_limits<double>::infinity();

        for(auto series : results) {
            for(auto xyResultPair : series) {
                auto val = xyResultPair.first;
                minX = std::min(minX, val);
                maxX = std::max(maxX, val);
            }
        }

        double diff = maxX - minX;
        double step = diff / (numTicks-1);
        double currentTick = minX;

        std::vector<double> ticks;
        ticks.reserve(numTicks);

        for(unsigned i=0; i<numTicks; ++i) {
            ticks.push_back(static_cast<unsigned>(currentTick));
            currentTick += step;
        }

        return ticks;
    }

    std::string getAxisHeader(const ResultMatrix& results,
                              std::string xLabel = "", std::string yLabel = "", std::string title = "") {

        std::stringstream axisHeader;
        axisHeader << "\\begin{axis}[";

        if(!title.empty())
            axisHeader << "title={" << m_caption << "},";

//        std::string legendText = "legend to name=" + cpptex::removeSpaces(m_caption)
//                                 + "-legend, legend columns={3}, ";

        axisHeader << "yticklabel style={rotate=90,anchor=base,yshift=0.2cm}, ";
        axisHeader << std::string("scaled ticks=false,grid=none,");

        if(!xLabel.empty())
            axisHeader << "xlabel={" + xLabel + "},";


        std::string xTicks;
        auto xTicksVec = getXTicks(results,0);
        double step = xTicksVec.size() > 1 ? xTicksVec[1] - xTicksVec[0] : 0.0;

        if(!xTicksVec.empty()) {
            axisHeader << "xtick={";

            for( auto val : xTicksVec)
                xTicks += std::to_string(val) + ",";

            xTicks = xTicks.substr( 0, xTicks.size()-1 );
            axisHeader << xTicks << "}, ";
            + "xmin="
            + std::to_string(xTicksVec.front() - step/2)
            + ",xmax="
            + std::to_string(xTicksVec.back() + step/2);
        }

        axisHeader << "ylabel near ticks,";

        if(!yLabel.empty())
            axisHeader << "ylabel={" + yLabel + "},";

        axisHeader << std::string("legend pos=north west,");

        axisHeader << "]"; // close axis environment attributes
        return axisHeader.str();
    }

    template<class ResultMap>
    std::string getAxisHeader(const std::string& iv, const ResultMap& results, const double xScale = 1.0,
                         const std::string xScaleUnit = "", bool first = true) {

        std::string axisHeader = std::string("")
                            + "\\begin{axis}[";
//            if(!m_caption.empty()) axisHeader += "title={" + m_caption + "},";
        std::string legendText = "legend to name=" + cpptex::removeSpaces(m_caption)
                + "-legend, legend columns={3}, ";
        axisHeader += "yticklabel style={rotate=90,anchor=base,yshift=0.2cm}, ";
        axisHeader += std::string("scaled ticks=false,grid=none,xlabel={$n$")
                      + (xScaleUnit.empty() ? "" : std::string(" (in ") + xScaleUnit + ")}, ")
                      + (first ? legendText : "")
                      + "xtick={";
        std::string xTicks = "";

        for( auto level : results.begin()->second ){
            xTicks += std::to_string(static_cast<double>(level.first) / xScale);
            xTicks += ",";
        }
        xTicks = xTicks.substr( 0, xTicks.size()-1 );


        auto firstTick = results.begin()->second.begin();
        auto secondTick = std::next(firstTick);
        auto lastTick = std::prev(results.begin()->second.end());

        auto firstTickScaled = static_cast<double>(firstTick->first) / xScale;
        auto secondTickScaled = static_cast<double>(secondTick->first) / xScale;
        auto lastTickScaled = static_cast<double>(lastTick->first) / xScale;

        double margin = std::abs(firstTickScaled - secondTickScaled);
        double xMin = firstTickScaled - margin;
        double xMax = lastTickScaled + margin;

        axisHeader += xTicks
                      + "\n}"
                      + ", ylabel near ticks,ylabel={"
                      + m_ivNiceNames.at(iv)
                      + "}, legend pos=north west,"
                      + "xmin="
                      + std::to_string(xMin)
                      + ",xmax="
                      + std::to_string(xMax);

        axisHeader += "]"; // close axis environment attributes
        return axisHeader;
    }

    std::string getAxisFooter(const std::string& plotLabel = "") {
        std::string axisFooter("\n\n\\end{axis}\n\n");
        axisFooter += "\\label{plots:"
                      + plotLabel
                      + "}";
        return axisFooter;
    }

private:
    static std::vector<std::string> MarkStyles;
    static std::vector<std::string> Marks;
    static size_t m_markIndex; // a valid index of Marks
    static std::string getMark() {
        auto currentMark = m_markIndex;
        m_markIndex = (m_markIndex+1) % Marks.size();
        return Marks.at(currentMark);
    }

    static std::vector<std::string> Colors;
    static size_t m_colorIndex;
    static std::string getColor() {
        auto currentColor = m_colorIndex;
        m_colorIndex = (m_colorIndex+1) % Colors.size();
        return Colors.at(currentColor);
    }
    static int m_markStyleCurrent;
    static std::string getMarkStyle() {
        std::string currentStyle = MarkStyles[m_markStyleCurrent];
        m_markStyleCurrent = (m_markStyleCurrent+1) % MarkStyles.size();
        return currentStyle;
    }
    static void addNewMarker(const std::string& algorithm) {
//            string color = getColor();
        std::string markerText;
//            markerText += "mark=" + getMark() + ","
//                    + "mark color=" + color + ","
//                    + "color=" + color;
        markerText = getMarkStyle();
        m_algorithmMarkers[algorithm] = markerText;
    }
    static std::map<std::string,std::string> m_algorithmMarkers;
    static std::string getMarkerText(const std::string& algorithm) {
        if(m_algorithmMarkers.find(algorithm) == m_algorithmMarkers.end())
            addNewMarker(algorithm);
        return m_algorithmMarkers.at(algorithm);
    }


    static std::map<std::string,std::string> m_ivNiceNames;
}; // class PgfplotsPrinter

int PgfplotPrinter::m_markStyleCurrent = 0;
std::map<std::string,std::string> PgfplotPrinter::m_algorithmMarkers;
std::vector<std::string> PgfplotPrinter::MarkStyles = {
        "color=black,mark options={fill=black},mark=square*",
        "color=black,mark options={fill=black},mark=pentagon*",
        "color=black,mark options={fill=black},mark=diamond*",
        "color=black,mark options={fill=black},mark=*",
        "color=black,mark options={fill=black},mark=triangle*",
        "color=black,mark=square",
        "color=black,mark=pentagon",
        "color=black,mark=diamond",
        "color=black,mark=o",
        "color=black,mark=otimes",
        "color=black,mark=triangle",
        "color=black,mark=oplus",
};
std::vector<std::string> PgfplotPrinter::Marks = {
        "otimes*",
        "oplus*",
        "o", "triangle", "pentagon", "square",  "diamond"
};
// Palette generated using https://coolors.co/
std::vector<std::string> PgfplotPrinter::Colors = {
        "000000",
        "2288DD"
//            "22dddd",
//            "2222dd",
//            "dd22dd",
//            "264653",
//            "1789a6",
//            "e76f51",
//            "33c1b1",
        //"287271",
//            "2a9d8f",
//            "e9c46a",
        //"f4a261",

};
size_t PgfplotPrinter::m_markIndex = 0;
size_t PgfplotPrinter::m_colorIndex = 1;

std::map<std::string,std::string> PgfplotPrinter::m_ivNiceNames = {
        {"runtime",             "Average execution time (s)"},
        {"degree",              "Maximum observed degree"},
        {"avgDegreePerPoint",           "Average observed degree per spanner"},
        {"avgDegreePerPoint",   "Average observed degree per point"},
        {"maxStretchFactor",    "Maximum observed stretch-factor"},
        {"avgStretchFactor",    "Average observed stretch-factor"},
        {"lightness",           "Average lightness"},
};


} // namespace cpptex


#endif //CPPTEX_PGFPLOTSPRINTER_H
