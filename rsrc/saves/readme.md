
Saved Game Format
=================

The Blades of Exile saved game format is a gzipped tarball with a .exg extension
and the following structure:

* save/
	* party.txt - Contains general info on the party.
	* pc*N*.txt - Contains info on active PC _N_, which is 1-6.
	* stored_pcs.txt - Contains a list of unique IDs for stored PCs. Optional.
	* pc~*N*.txt - Contains info on stored PC _N_, which must be found in _stored_pcs.txt_.
	* export.png - Contains graphics referenced from other files in this folder. Optional.
	* scenario.txt - Contains some persistent information about towns in the scenario.
	* setup.dat - Contains saved information about fields in recent towns you've visited.
	* town.txt - Contains information about the current town, if the party is in down.
	* townmaps.dat - Contains information about what areas of towns have been explored.
	* out.txt - Contains information about the current outdoor region (a 2x2 grid of sectors).
	* outmaps.dat - Contains information about what areas of the outdoors have been explored.

A party that is not in a scenario omits all the files from _scenario.txt_ down.
The _stored_pcs.txt_ file is only included if there actually are stored PCs.
The _town.txt_ file is omitted if the party is outdoors, but _townmaps.dat_ is not.

If you have a _save_ directory with the proper structure, it can be turned into a
valid saved game with the following shell command:

	tar -zcf save.exg save

(This should work on the Windows command-line too, assuming you have `tar.exe` installed
and in the `%PATH%` or current directory.)

Most of the files in the saved game use a "tag file" format, which is described below. There are four exceptions (plus export.png):

* The _stored_pcs.txt_ file is simply a list of numbers, one per line. It's essentially an index of the stored PCs – for every number in this file, there will be a corresponding _pc~N.txt_.
* The _townmaps.dat_ file is simply a list of bitfields, one per line, addressing all towns in the scenario in order of definition from top to bottom. Each bitfield covers one row of one town.
* The _out.txt_ file is a straight dump of the active outdoor terrain, two consecutive 96x96 grids of numbers separated by spaces and newlines. The first set covers the terrain while the second set covers the explored flag.
* The _outmaps.dat_ file is simply a list of bitfields, several per line, addressing all outdoor sectors in order of definition from top to bottom. Each line covers one row of every sector in a single row of the outdoors grid, so for example if the outdoors is 3x3, then each row will contain 3 bitfields addressing a row of sectors (_N_,0), (_N_,1), and (_N_,2).

Tag Files
=========

The "tag file" format is very simple. A "tag file" consists of a series of "pages", separated by formfeed characters. Each page consists of a series of "tags", one per line. Each tag consists of an "identifier" followed by zero or more "values", separated by whitespace. Values can be quoted if they contain spaces or tabs; this can also convey a blank value. The order of tags is preserved and can be significant. Sometimes the mere presence of a tag conveys a meaning – tags without values are often used for this purpose. There are no restrictions on what characters can appear in an identifier, other than whitespace (quotes are not parsed in the identifier).

When parsing tag values, quotes (" and ') only have a special meaning if they appear as the first character. So, for example, the string "don't" would not need to be quoted, as it doesn't start with '. The following backslash escape sequences are supported in quoted strings:

* `\\` - Literal backslash
* `\'` - Literal single quote (only required in single-quoted strings)
* `\"` - Literal double quote (only required in double-quoted strings)
* `\n` - A newline
* `\t` - A horizontal tab (not required, but parsed for consistency)
* `\f` - A formfeed

Note that no escape sequences will be expanded in unquoted strings.
For tag values that are interpreted as booleans, the strings "true" and "false" are used.
Most tag values interpreted as numbers use the decimal format, but a hexadecimal format
is also supported. Which format to use is decided by the code that loads the file.

For example, a very simple two-page tag file might look like this:

	NAME "It's Me!"
	LEVEL 5
	HEALTH 12 34
	MANA 1 45
	<formfeed>
	ITEM 0
	NAME "Broken Sword"
	DAMAGE 2 7
	EQUIPPED
	<formfeed>
	ITEM 1
	NAME "Healing Potion"
	ABILITY heals

An actual tag file would not add a newline between the formfeed and the following tag.

For the tag files actually used in the saved game, pages are usually identified based on
the first tag that appears in them. However, the very first page is usually treated
specially without checking what its first tag is.

The exact list of pages and tags that are used in the saved game files are not yet
finalized, so they won't be documented here. However, it is not hard to figure it out
by looking at the code – look for functions called `writeTo` or `readFrom` that take
either a `cTagFile` or a `cTagFile_Page` as a parameter.
