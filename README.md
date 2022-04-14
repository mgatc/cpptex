# cpptex
C++ library for printing latex documents, including tikz images, pgfplots, graphs, and tables.

To get started, clone cpptex into the desired directory and include `cpptex.h` where you want to use a printer.

Available printers are 

* `LatexPrinter` - general latex document printer
* `TikzPrinter` - general tikz image printer
* `PgfplotsPrinter` - print plots e.g. "line graphs"
* `GraphPrinter` - vertices, edges, etc.
* `TablePrinter` - print tabular data

See implementations in `cpptex/detail/` for more info.
