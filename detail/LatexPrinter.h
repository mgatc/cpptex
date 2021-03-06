#ifndef CPPTEX_LATEXPRINTER_H
#define CPPTEX_LATEXPRINTER_H

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "util.h"

namespace cpptex {

/**
 * The base printer class. Provides a good outer shell of a document and basic functionality
 * applicable to all Latex documents, such as writing the .tex file, adding other printers as
 * subdocuments or figures, compilation, and display.
 */
class LatexPrinter {
public:
    typedef std::pair<std::string,std::string> Option;
    typedef std::vector<Option> OptionsList;


    explicit LatexPrinter(std::string path, std::string documentType = "article")
        : m_documentType(std::move(documentType))
    {
        std::tie(m_directory, m_filename) = splitDirectoriesFromFilename(path);
    }

    // Document-level getters
    std::string getName() const {
        return m_filename;
    }
    std::string getFullDocumentText() const {
        return getDocumentHeader()
               + getBodyText()
               + getDocumentFooter();
    }
    std::string getBodyText() const {
        return m_body.header + m_body.content + m_body.footer;
    }
    std::string getDocumentHeader() const {
        std::string header = "\\documentclass{"
                + m_documentType
                + "}\n\n"
                  + "\\usepackage[table]{xcolor}\n"
                + "\\usepackage{tikz,pgfplots,amsmath,fullpage,rotating}\n"
                + "\\usetikzlibrary{shapes}\n"
                + "\\pgfplotsset{compat=1.15}\n\n"
                + getColorDefinitions()
                + "\n\n\n\n"
                + "\\begin{document}\n\n";
        return header;
    }
    static std::string getDocumentFooter() {
        return "\\end{document}";
    }
    // Figure getters
    static std::string getFigureHeader(bool subfigure = false, double ratioOfLineWidth = 1.0, std::string options = "") {
        std::string header;
        if(subfigure) {
            header = std::string("\\begin{minipage}{")
                    + std::to_string(ratioOfLineWidth) + "\\linewidth}";
        } else {
            header = std::string("\\begin{figure}[ht]");
            if(!options.empty()){
                header += "[" + options + "]";
            }
            header += "\n\\centering\n\n";
        }
        return header;
    }
    std::string getFigureFooter(bool subfigure = false) const {
        std::string footer;
        if(subfigure) {
            footer += std::string("\\end{minipage}");
        } else {
            footer += std::string("\\end{figure}\n");
        }
//            if( !m_caption.empty() ) {
//                footer += string("")
//                    + "\\caption{"
//                    + m_caption
//                    + "}\n";
//            }
        return footer;
    }
    // Figure captions
    std::string getCaption() const {
        return "\\caption{"
            + m_caption
            + "}\n";
    }
    void setCaption(const std::string& caption ) {
        m_caption = caption;
        addComment(caption);
    }
    template<class T>
    void setCaption(const T& setter) {
        setter.setCaption(this);
    }
    static void clearCaptionFile() {
        std::string captionFilename = "captions.txt";

        FILE *fp = fopen(captionFilename.c_str(), "w");
        fprintf(fp, "%s", "");
        fclose(fp);
    }
    // Adding to the document's body
    /** Add the contents of a LatexPrinter object to this document*/
    void addToDocument(const LatexPrinter& printer, const bool precompile = false) {
        std::string includeName = printer.getName();
        if(precompile) {
            printer.compile();
            includeName += ".pdf";
            addGraphic(includeName);
        } else {
            printer.saveBody();
            includeName += m_bodySuffix;
            addInput(includeName);
        }


        for( const auto& hex : printer.m_colors ) {
            defineColor(hex);
        }
    }
    /** Add the contents of a LatexPrinter object to this document as a figure. */
    void addToDocumentAsFigure(const LatexPrinter& printer, const bool precompile = false, const bool captionAbove = true) {

        addRawText(getFigureHeader(false));

        std::string caption = printer.getCaption();
        if(!caption.empty() && captionAbove)
            addRawText(caption);

        addToDocument(printer,precompile);

        if(!caption.empty() && !captionAbove)
            addRawText(caption);

        addRawText(getFigureFooter(false));
    }
    int m_numSubfigs = 0;
    void addToDocumentAsSubfigure(const LatexPrinter& printer, int numCols = 3, std::string caption="", bool precompile = false) {

        addRawText(getFigureHeader(true, 0.95/numCols, caption));
        addToDocument(printer,precompile);
        addRawText(getFigureFooter(true));

        ++m_numSubfigs;

        std::string separator = m_numSubfigs % numCols == 0 ? "\n\n" : "\n\\hfill\n";
        addRawText(separator);

    }
    void addInput(const std::string& name) {
        m_body.content += "\\input{" + name + "}\n\n";
    }

    void addGraphic(const std::string& name) {
        m_body.content += "\\includegraphics{" + name + "}\n\n";
    }
    void addRawText(const std::string& text) {
        m_body.content += text;
    }
    void clearpage() {
        addRawText("\\clearpage\n\n");
    }
    void addComment(const std::string& comment ) {
        std::stringstream str(comment);
        std::string line;
        while(std::getline(str, line)) {
            addRawText("% " + line + "\n");
        }
    }

    void save() const {
        std::string texFilename = m_directory + getTexFilename();
        std::cout<<"Saving file "<<texFilename<<"..."<<std::flush;

        FILE *fileOut = fopen(texFilename.c_str(), "w");

        fprintf(fileOut, "%s", getFullDocumentText().c_str());

        fclose(fileOut);
        std::cout<<"done."<<std::endl;
    }
    void saveBody() const {
        std::string texFilename = m_directory + getTexFilenameForBody();
        std::cout<<"Saving file "<<texFilename<<"..."<<std::flush;

        FILE *fileOut = fopen(texFilename.c_str(), "w");

        fprintf(fileOut, "%s", getBodyText().c_str());

        fclose(fileOut);
        std::cout<<"done."<<std::endl;
    }
    std::string m_compiler = "pdflatex";
    void compile() const {
        save();

        std::cout<<"Compiling "<< getTexFilename()<<"..."<<std::flush;
        std::string command = m_compiler + " -output-directory=" + m_directory
                        + " " + m_directory + getTexFilename() + " > /dev/null";
        std::ignore = system(command.c_str());
        std::cout<<"done."<<std::endl;
    }
    std::string m_viewer = "evince";
    void display() const {
        compile();

        std::cout<<"Opening "<<getPdfFilename()<<" for viewing."<<std::endl;
        std::string command = m_viewer + " " + m_directory + getPdfFilename() + " &";
        std::ignore = system(command.c_str());
        std::ignore = system(command.c_str());
    }
    /** Define a hex rgb color for use in the document. */
    void defineColor( const std::string& hex ) {
        // add color to colormap
        m_colors.insert( hex );
    }

protected:
    struct Body {
        std::string header;
        std::string content;
        std::string footer;
    };
    Body m_body;
    std::string m_directory;
    std::string m_filename;
    std::string m_documentType;
    std::string m_caption;
    std::unordered_set<std::string> m_colors;
    std::string m_bodySuffix = "_body";

    std::string getTexFilename() const {
        return m_filename + ".tex";
    }
    std::string getTexFilenameForBody() const {
        return getName() + "_body.tex";
    }
    std::string getPdfFilename() const {
        return m_filename + ".pdf";
    }
    static std::string expandOptions( const OptionsList& options ) {
        std::string optionsString;
        for( auto& o : options ) {
            optionsString += o.first
                             + ( !o.second.empty()?("=" + o.second):"") // include second param if given
                             + ",";
        }
        // remove trailing comma
        return optionsString.substr( 0, optionsString.size()-1 );
    }

    std::string getColorDefinitions() const {
        std::string definitions;
        for( const auto& hex : m_colors ) {
            // parse the hex value
            std::vector<size_t> color = parseHexRGB( hex );
            // add color to document
            definitions += "\\definecolor{"
                           +  hex + "}{RGB}{ "
                           +  std::to_string(color.at(0)) + ", "
                           +  std::to_string(color.at(1)) + ", "
                           +  std::to_string(color.at(2))
                           + " }\n";
        }
        definitions += "\n";
        return definitions;
    }
};

} // cpptex

#endif //CPPTEX_LATEXPRINTER_H
