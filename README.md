Lua/C Markov Babbler
======

Dependencies:
* lua5.2
* liblua5.2

`make run` will run babbler on included text file (*The Count of Monte Cristo* by Alexandre Dumas, from Project Gutenberg) with a default wordcount of 200 and n-gram size of 4.

Else use with `./babbler <filename> [wordcount] [n-gram size]`

Keeps:
* Accented characters regardless of locale

Culls:
* Page numbers of the format "12345m"
* Chapter headings
* Extended punctuation like left/right double/single quotes and long dashes

All without prior modification of input file save removal of Proj Gutenberg header/footers
