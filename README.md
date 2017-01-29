# Markdowntown

This project contains a reference implementation for ''markdowntown'' markup language. The ''markdowntown'' is based on traditional [markdown](http://daringfireball.net/projects/markdown/syntax) languages and [Wiki](https://en.wikipedia.org/wiki/Help:Wiki_markup) markup language.

This implementation still under development, but you can already use the `markdowntown` tool to process the basic language statements. The tool is able to convert markdowntown markup language to HTML.

## Building

Markdowntown uses CMake. To build the tool, you must clone the repository in you machine, open the terminal in that directory and use the following commands:

    # mkdir build
    # cd build
    # cmake ..
    # make

This way you will compile the static library `libmarkdowntown` and the tool `markdowntown`.

## Usage

To process a document, use the tool `markdowntown` as follows:

    # ./markdowntown document.txt > document.htm

If the input file contains any syntax errors, the tool will print some information in the screen (`stderr`).

## Syntax

Here is presented a few markdowntown commands and statements. This section is just an initial guide.

### Paragraphs

A paragraph is sequence of one or more consecutive lines. To end a paragraph, just insert blank line (i.e. line that contains no characters or only white spaces).

    This is a paragraph.
    Still the same paragraph

    Another paragraph.


### Lists

Markdowntown offers two types of lists: ordered and unordered. Ordered lists starts with asterisk (*) and became the HTML `ol` element. Unordered lists starts with sharp (#) and became the HTML `ul` element. You can insert many statements inside a list entry using continuation: start the list item in one line and start the next line with a tabulation or four spaces. You can create many levels of continuation.

    * List item
        * Another list item
            *   This item is in another level and
                will continue in this line.
            Some more text.

### Hyperlinks

Links are created using the following syntax:

    [ Text to be rendered ] ( URL )

Trailing spaces in the text to be rendered and in the URL will be removed.

### Macros

Macros works like functions and allows the user to perform special tasks at some point of the document. Macros can have zero ou more parameters. Parameter identifiers can be omitted.

The basic syntax for macros is:

    {{ MacroName | parameter = value | value | ... }}

The following macros are available until now:

* **{{TOC}}** Generate a table of contents.
* **{{StyleSheet | **_file.css_**}}** Insert a reference to some CSS file in the output HTML.
* **{{Format |**_CSS classes_ **|** _text_ **}}** Insert a HTML `span` element using the given CSS classes and containing the specified text.

## License

Except where explicitly indicated otherwise, all source codes of this project are provide under [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).