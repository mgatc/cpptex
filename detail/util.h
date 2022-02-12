#ifndef CPPTEX_TIKZPRINTER_H
#define CPPTEX_TIKZPRINTER_H

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <string>

namespace cpptex {

    const bool PRECOMPILE_SUBDOCUMENTS = true;

    const double PI = M_PI;

    // see https://stackoverflow.com/questions/5891610/how-to-remove-certain-characters-from-a-string-in-c
    std::string removeCharsFromString( std::string str, const char* charsToRemove ) {
        for ( unsigned int i = 0; i < std::strlen(charsToRemove); ++i ) {
            str.erase( remove(str.begin(), str.end(), charsToRemove[i]), str.end() );
        }
        return str;
    }
    std::string removeSpaces(std::string str) {
        const char space = ' ';
        return removeCharsFromString(str,&space);
    }


    struct SetPrecision {
        int value;
        std::string operator()(const std::string& val) const {
            if(value < 0)
                return val;

            try {
                double numVal = stod(val);
                return (*this)(numVal);
            } catch(std::invalid_argument& ia) {
                return val;
            }
        }
        std::string operator()(const double& val) const {
            if(value < 0) {
                return std::to_string(val);
            }
            std::stringstream str;
            str << std::fixed;
            str << std::setprecision(value);
            str << val;
            return str.str();
        }
    };

    std::string texttt(const std::string& input) {
        return "\\texttt{" + input + "}";
    }
    std::string subsection(const std::string& input) {
        return "\\subsection{" + input + "}";
    }
    std::string mathrm(const std::string& input) {
        return "\\mathrm{" + input + "}";
    }

    std::vector<size_t> parseHexRGB( const std::string& hex_str ) {
        // the hex string should contain 6 digits
        // three 2-digit hex numbers
        std::vector<size_t> rgb(3, 0);
        // parse each 2-digit number and convert to base 10
        for( size_t i=0; i<3&&i<hex_str.size()/2; ++i ) {
            rgb[i] = stoi( hex_str.substr(2*i, 2), 0, 16 );
        }
        return rgb;
    }

} // namespace cpptex

#endif // CPPTEX_UTIL_H
