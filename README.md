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


### Headers

Define titles for sections of the document. There is 6 levels of headers, determined by the number of `=` inserted. Each header level is converted to its corresponding HTML header element (e.g. `==` became `h2`).

    = This is a level 1 header

    == This is a level 2 header

    ==== This is a level 4 header ==

It's allowed to "close" headers putting `=` in the end. The closing symbols will be ignored.


### Blockquote

Markdowntown uses e-mail style blockquotes. To blockquote a section of text, just put a `>` character followed by a white space (the white space is required, unless the line is empty).

    > This text is inside a blockquote
    >
    > This one is in the same blockquote
    > > Blockquote inside a blockquote

A blockquote is closed when you put an empty line or another statement after it.


### Lists

Markdowntown offers two types of lists: ordered and unordered. Ordered lists starts with asterisk (*) and are converted to HTML `ol` elements. Unordered lists starts with sharp (#) and are converted to HTML `ul` elements. You can insert many statements inside a list entry using **continuation**: start the list item in one line and start the next line with a tabulation or four spaces. You can create many levels of continuation.

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

Macros works like functions and allows the user to perform special tasks at some point of the document. Macros are identified by name and can have zero or more parameters. Parameter identifiers can be omitted if the values are given in the correct order.

The basic syntax for macros is:

    {{ MacroName | parameter = value | value | ... }}

The following macros are available for now:

`{{TOC}}`
Generate a table of contents.

`{{StyleSheet | file.css }}`
Insert a reference to some CSS file in the output HTML.

`{{Format | CSS classes | text }}`
Insert a HTML `span` element using the given CSS classes and containing the specified text.


### Raw text

This feature allows you to insert raw content in the HTML output. You can use it to insert custom HTML statements (e.g. `div`, scripts, inline stylesheets, stc).

    = Raw text example

    To insert raw text, use the `{{{` and `}}}` operators.

    {{{
        <div>This is a div</div>  }}}

Everything that is between the `{{{` and `}}}` operators will be written to the HTML output without any processing.


### Special CSS classes

Some markdowntown statements are able to include special CSS classes based in the amount of symbols you use. This allows you more control about how the elements will be rendered.

Blockquotes, for example, can be specified using more than one `>` in the same declaration.

    > Blockquote with CSS class 'type1'

    >> Blockquote with CSS class 'type2'

    >> > Blockquote 'type1' inside a blockquote 'type2'

You can use this feature with lists too.

    * List of 'type1'
        ** List of 'type2'

    *** List of 'type3'

When you use something like

    >> setterm -blength 0

the output HTML will be

    <blockquote class='type2'>setterm -blength 0</blockquote>

You could create a CSS class in your stylesheet to use this type of blockquote as shell output.


## License

Except where explicitly indicated otherwise, all source codes of this project are provide under [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).