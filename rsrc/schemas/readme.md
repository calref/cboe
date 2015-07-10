
Blades of Exile New Scenario Format
===================================

This document describes, in as much detail as possible, the format of _.boes_ files. Parts
of the format are specified by the various XSD schemas in this directory, but the schemas
are not a perfect representation of the format - they are a little more restrictive than
the game itself accepts, and omit a few complex restrictions that the game enforces.

The Scenario Package
--------------------

A Blades of Exile scenario is a tarball (ustar) compressed with gzip. The tarball contains
the following files:

* scenario/
	* header.exs - This file has no function except to allow you to open a scenario that
	has been unpacked with the scenario editor. It contains the scenario and Blades of Exile
	version in binary format.
	* scenario.xml - Contains most of the global data for the scenario.
	* scenario.spec - Contains the definitions of the scenario special nodes.
	* terrain.xml - Contains the definitions of scenario terrain types.
	* items.xml - Contains the definitions of scenario items.
	* monsters.xml - Contains the definitions of scenario monsters.
	* towns/
		* town*N*.xml - Contains most of the data for town _N_.
		* town*N*.spec - Contains the definitions of the special nodes for town _N_.
		* town*N*.map - Contains the map data for town _N_.
		* talk*N*.xml - Contains the dialogue nodes associated with town _N_.
	* out/
		* out*X*~*Y*.xml - Contains most of the data for outdoors sector (_X_,_Y_).
		* out*X*~*Y*.spec - Contains the definitions of the special nodes for outdoors
		sector (_X_,_Y_).
		* out*X*~*Y*.map - Contains the map data for outdoors sector (_X_,_Y_).
	* graphics/ - (optional)
		* sheet*X*.png - (optional) Custom graphics sheet ID _X_.
		* objects.png, tinyobj.png, fields.png, trim.png, booms.png, missiles.png,
		dlogpics.png, staticons.png, teranim.png, talkportraits.png, pcs.png, vehicle.png,
		termap.png - (optional) Overrides the default graphic sheets of the same name.
	* sounds/ - (optional)
		* SND*X*.wav - (optional) A custom sound with ID _X_. If _X_ < 100, the sound will
		override the default sound with the same ID.
	* dialogs/ - (optional) - Contains complex dialog definitions used by the scenario.
	This is currently unused.

If you have a _scenario_ directory with the proper structure, it can be turned into a
valid scenario with the following shell command:

	tar -zcf scenname.boes scenario

Special Nodes
-------------

Scenario special nodes are stored in _.spec_ files in a special "pseudo-assembly" format.
The grammar is as follows:

	«node-file» -> («comment» | «const-def» [«comment»] | «node-def» [«comment»])*
	«const-def» -> def «identifier» = «integer»
	«node-def» -> @«opcode» [= «integer»] «data-def»*
	«data-def» -> «data-def-unary» | «data-def-binary» | «data-def-ternary»
	«data-def-unary» -> goto = «value»
	«data-def-binary» -> (sdf | pic) = «value» [, «value»]
	«data-def-ternary» -> (msg | ex1 | ex2) = «value» [, «value» [, «value»]]
	«value» -> «integer» | «identifier»

An `«identifier»` is a sequence of letters, underscores, hyphens, and dollar signs (digits
not permitted). An `«integer»` is a sequence of digits. An `«opcode»` is a special
identifier; valid opcodes are listed in the file
[specials-opcodes.txt](../strings/specials-opcodes.txt).
In addition, `nop` is also a valid opcode.

A `«comment»` is any sequence of characters starting with the `#` character and ending
with a newline.

Note that, although the format supports defined constants using `def`, the scenario never
writes defined constants.

Map Data
--------

Map data is stored in _.map_ files in a two-dimensional format. Each line in the file
represents one row of terrain in the town or outdoor sector. Individual tiles within a
single row are comma-separated. A tile consists of an integer, giving the terrain type on
that space, followed by an optional set of modifiers. Most modifiers consist of a special
character followed by an integer. A few modifiers consist of only a special character.
There can be multiple modifiers of the same type on a single tile.

The list of valid tile modifiers follows:

* `^ < > v` - Gives the position of a town entrance from one specific direction. (The
fourth one there is lowercase V.) Takes no argument. Only valid in town.
* `*` - Specifies that the tile is a wandering monster arrival point. Argument is which
point (0-3).
* `:` - Indicates a special node is attached to the space. Argument is the node number.
* `!` - Indicates a sign is attached to the space. Argument is which sign.
* `@` - When used outdoors, indicates a town entrance; argument is the town number. When
used in town, indicates a placed item; argument is the index of the placed item entry.
* `&` - Indicates an overlay such as a field, object, or special spot. The argument is a
number indicating the type of field (valid values are listed below). It can be used
outdoors, but the only valid argument is `9`, indicating a special spot.
* `$` - When used in town, indicates a placed creature; argument is the index of the
placed creature entry. When used outdoors, indicates a placed special encounter group;
argument is the index of the special encounter. (Note: Placed outdoor special encounters
are not yet supported.)
* `h` - Indicates a horse that belongs to the party. Argument is the index of the horse in
the global list.
* `H` - As above, except the horse does not belong to the party.
* `b` - Indicates a boat that belongs to the party. Argument is the index of the boat in
the global list.
* `B` - As above, except the boat does not belong to the party.

General XML notes
-----------------

All XML documents used in the scenario format require a `boes` attribute on their root
node, which should be set to `2.0.0`. This is a way to indicate the _format version_; if
the format ever changes, the value of this attribute will also be changed.

Some elements that contain text content are automatically wrapped in a `CDATA` declaration
by the scenario editor. The rest are never wrapped in a `CDATA` declaration. As a result,
while the game will correctly load `CDATA` declarations in any text element, it's only
safe to use them in elements where the scenario editor would use one.

There are some types that are common:

* Booleans - a boolean is one of the strings "`true`" or "`false`".
* Points - a point is an element with two attributes, `x` and `y`. It has no content.
* Rects - a rect is an element with four attributes: `top`, `left`, `bottom`, and `right`.
It has no content
* Timers - a timer is an element whose content is the number of the node to call, with an
attribute `freq` specifying how many moves to wait between calls.

Scenario Header
---------------

The _scenario.xml_ file stores most of the global scenario data. The root element is
`<scenario>`, and the following toplevel tags are required:

* `<title>` - Contains the scenario name.
* `<icon>` - Contains an integer specifying the scenario icon.
* `<id>` - Contains the scenario's campaign ID.
* `<version>` - Contains the scenario version.
* `<language>` - For future expansion. Currently always contains "en-US".
* `<author>` - Contains information about the scenario author. Requires two sub-elements:
`<name>` and `<email>`.
* `<text>` - Contains basic textual information about the scenario. Can contain any of the
following:
	* `<teaser>` - (max 2) A short string to be displayed in the scenario selection
	dialog.
	* `<icon>` - Overrides the toplevel `<icon>` just for the intro dialog.
	* `<intro-msg>` - (max 6) A string to be shown in the intro dialog. If none are
	present, or all are empty, no intro dialog is shown. The scenario editor wraps the
	contents of this element in a `CDATA` declaration.
* `<ratings>` - Contains the scenario rating. The following two subtags are required:
	* `<content>` - The content rating. Must be one of `G`, `PG`, `R`, `NC-17`.
	* `<difficulty>` - The difficulty rating. Must be an integer from 1 to 4.
* `<flags>` - Contains various scenario flags.
* `<creator>` - Contains information about the program that wrote this scenario. Requires
the following subtags:
	* `<type>` - Must contain the string `oboe`. If you fork OBoE and change the scenario
	format, you would use a different string here to let mainline OBoE know that it's
	incompatible.
	* `<version>` - The version of OBoE that the scenario was last edited with.
	* `<os>` - The operating system on which the scenario was last edited.
* `<game>` - Contains the bulk of global information about the scenario.
* `<editor>` - Contains several details of the scenario that aren't relevant to gameplay,
but are instead used by the scenario editor to enhance your editing experience.

The `<game>` element can have the following child tags, most of which are optional:

* `<num-towns>` - (required) Indicates how many towns the scenario has.
* `<out-width>`, `<out-height>` - (required) Indicates how many outdoor sectors the
scenario has.
* `<start-town>` - (required) Specifies which town the player should start in.
* `<town-start>` - (required) Specifies the player's initial position in the start town,
as a point.
* `<outdoor-start>` - (required) Specifies which outdoor section the player should start
in, as a point.
* `<sector-start>` - (required) Specifies the player's initial position in the starting
sector.
* `<on-init>` - Indicates the number of a scenario special node to call when the scenario
first starts.
* `<store-items>` - (max 3) Specifies a rectangle from which the player can retrieve items
after completing the scenario, as a rectangle with an extra `town` attribute that
specifies which town the area
* `<town-flag>` - (max 10) Specifies a rule to add the value of an SDF to a town upon
entry. The SDF is specified with the `add-x` and `add-y` attributes, and the town number
with the `town` attribute.
* `<special-item>` - (max unbounded) Defines one special item. Special items must appear
in order, starting with special item ID 0. Special items have the following attributes and
subtags:
	* `start-with=` - Boolean indicating whether the party starts the scenario with this
	item.
	* `usable=` - Boolean indicating whether the item can be used.
	* `special=` - (required only if `usable="true"`) Indicates the special node to call
	when the special item is used.
	* `<name>` - (required) The name of the special item.
	* `<description>` - (required) The special item description. The scenario editor wraps
	the contents of this element in a `CDATA` declaration.
* `<quest>` - (max unbounded) Defines one quest. Quests must appear in order, starting
with quest ID 0. Quests have the following attributes and subtags:
	* `start-with=` - Boolean indicating whether the party starts the scenario with this
	quest.
	* `<deadline>` - (optional) Specifies the quest deadline (in days) as its content.
	Supports two optional attributes - `waive-if`, an integer specifying the event whose
	occurrence waives the deadline, and `relative`, a boolean indicating whether the
	deadline is relative to the day the quest is started.
	* `<reward>` - (optional) An empty element specifying the reward for completing the
	quest, with its two possible attributes `gold` and `xp`.
	* `<bank>` - (optional, max 2) Specifies that the quest belongs to one or two job
	banks.
	* `<name>` - (required) - The name of the quest.
	* `<description>` - (required) The quest description. The scenario editor wraps the
	contents of this element in a `CDATA` declaration.
* `<shop>` - (max unbounded) Defines one shop. Shops must appear in order, starting with
shop ID 0. Shops require the following subtags:
	* `<name>` - The name of the shop, used by the scenario editor only.
	* `<type>` - The type of the shop. Must be one of `live`, `dead`, `rand`.
	* `<prompt>` - The shopping prompt. Must be one of `shop`, `heal`, `mage`, `priest`,
	`spell`, `alch`, `train`.
	* `<face>` - The talking portrait to use in the shop.
	* `<entries>` - The actual contents of the shop. It can contain any of the following
	elements in any order, but no more than 30 in total:
		* `<item>` - An ordinary item. The content specifies which item (taken from the
		scenario's item list). By default, there's an infinite amount of this item; the
		attribute `quantity` can specify a different amount, or you can explicitly specify
		`quantity="infinite"`. You can also use the `chance` attribute to make the item
		appear only sometimes (defaults to 100). Note: If you specify the `chance`
		attribute to be something other than 100, the shop's type needs to be set to
		`rand` for it to work properly.
		* `<mage-spell>` - A mage spell. The content specifies which item (0-61).
		* `<priest-spell>` - A mage spell. The content specifies which item (0-61).
		* `<recipe>` - A mage spell. The content specifies which item (0-19).
		* `<skill>` - A mage spell. The content specifies which item (0-20).
		* `<treasure>` - A random item of a specific treasure class (0-4). Note: If you
		use this type of entry, the shop's type needs to be set to `rand` for it to work
		properly.
		* `<class>` - A random item of a specific special class (1-100). Note: If you use
		this type of entry, the shop's type needs to be set to `rand` for it to work
		properly.
		* `<heal>` - Healing. Must be one of the following:
			* `0` - Restore health to full.
			* `1` - Remove poison.
			* `2` - Remove disease.
			* `3` - Remove acid.
			* `4` - Remove paralysis.
			* `5` - Uncurse all carried items.
			* `6` - Revive petrified character.
			* `7` - Revive dead character.
			* `8` - Revive obliterated (dust) character.
			* `9` - Remove dumbfounding.
		* `<special>` - An item that calls a special node when purchase. Requires the
		following subtags:
			* `<name>` - The name of the entry.
			* `<description>` - A description of the entry. The scenario editor wraps the
			contents of this element in a `CDATA` declaration.
			* `<node>` - The scenario special node to call.
			* `<quantity>` - How many times the item can be purchase (an integer or the
			special value `infinite`).
			* `<cost>` - How much the item costs to purchase.
			* `<icon>` - The item icon to use.
* `<timer>` - (max 20) A scenario timer.
* `<string>` - (max unbounded) A scenario string. The scenario editor wraps the contents
of this element in a `CDATA` declaration.
* `<journal>` - (max unbounded) A journal string. The scenario editor wraps the contents
of this element in a `CDATA` declaration.

The `<editor>` element can have the following child tags:

* `<default-ground>` - (required) The default ground used for new areas. Currently this is
set to 0 or 2 at scenario creation, and no way to change it is offered in the scenario
editor UI.
* `<last-out-section>` - (required) The most-recently-edited outdoor sector, as a point.
* `<last-town>` - (required) The number of the most-recently-edited town.
* `<graphics>` - Stores the classification of your custom graphics. For each classified
graphic, a `<pic>` child element is used, with an `index` property specifying the number
of the graphic (_sheet_ * 100 + _row_ * 10 + _col_). The content of this element specifies
the type of graphic as an integer; the following values are valid:
	* 0 or 11 - This specifies that the graphic is either unused or part of another
	graphic that starts in an earlier slot. However, this is also the default for any
	slots that don't have a `<pic>` element referencing them.
	* 1 - Specifies that the slot contains a non-animated terrain graphic.
	* 2 - Specifies that the slot contains the first from of an animated terrain graphic.
	* 15 - Specifies that the slot contains six terrain map icons.
	* 3 - Specifies that the slot contains the first frame of a single-space monster.
	* 23 - Specifies that the slot contains the left half of the first frame of a wide
	two-space monster.
	* 43 - Specifies that the slot contains the top half of the first frame of a tall
	two-space monster.
	* 63 - Specifies that the slot contains the top left quadrant of the first frame of a
	four-space monster.
	* 4 - Specifies that the slot contains the left half of a dialog graphic.
	* 13 - Specifies that the slot contains the left half of the top left quadrant of a
	dialog graphic.
	* 5 - Specifies that the slot contains the left half of a talk portrait.
	* 7 - Specifies that the slot contains a full-space item graphic.
	* 10 - Specifies that the slot contains the first frame of an animated explosion.
	* 12 - Specifies that the slot contains the first two frames of a missile animation.
* `<storage>` - (max 10) Defines one quick item placement shortcut. It has the following
subtags:
	* `<on-terrain>` - Specifies the terrain type to which this shortcut applies.
	* `<is-property>` - A boolean specifying whether the items in this shortcut should
	belong to someone other than the party.
	* `<item>` - (max 10) Specifies which item to place. It has a required `chance`
	attribute to give the chance of placing this item.
* `<sound>` - (max unbounded) Gives a name for a custom sound (ID 100 or greater). These
names are used when showing the Pick Sound dialog in various places. The required `id`
attribute specifies which sound it applies to.

Terrain Types
-------------

The _terrain.xml_ file stores the definitions of all terrain types in the scenario. The
root element is `<terrains>`, and the only toplevel tag is `<terrain>`, of which there can
be any number. The `<terrain>` element also has a required `id` attribute which must be
unique. The following tags are allowed in a `<terrain>` element:

* `<name>` - (required) The name of the terrain type.
* `<pic>` - (required) The terrain's graphic number.
* `<map>` - (required) The terrain's map graphic.
* `<blockage>` - (required) The terrain's blockage level. Must be one of `none`, `move`,
`monsters`, `sight`, `move-and-shoot`, `move-and-sight`.
* `<special>` - (required) The terrain's special ability. This element requires a `<type>`
subtag which holds a string representing the type of ability (for a list of valid values,
search for `eTerSpec` in [estreams.cpp](../../src/classes/estreams.cpp)). It can also
contain up to three `<flag>` fields which hold the integer values of the three terrain
special flags, in order.
* `<transform>` - The terrain type to transform to when a Transform special node is
invoked.
* `<fly>` - A boolean indicating whether the terrain can be flown over.
* `<boat>` - A boolean indicating whether the terrain can be boated over.
* `<ride>` - A boolean indicating whether horses can be ridden across the terrain. Note
that this is the inverse of the "Blocked to Horses" flag in the scenario editor.
* `<archetype>` - A boolean indicating whether this terrain should be considered the
archetype for its ground type. There should be exactly one archetype for a given ground
type.
* `<light>` - An integer specifying the radius of light emitted by this terrain.
* `<step-sound>` - The sound to play when you step on this terrain. Must be one of `step`,
`crunch`, `squish`, `splash`, `none`.
* `<trim>` - (required) The trim type of this terrain. Must be one of `none`, `wall`, `s`,
`n`, `e`, `w`, `sw`, `ne`, `se`, `nw`, `sw-inner`, `ne-inner`, `se-inner`, `nw-inner`,
`frills`, `road`, `walkway`, `waterfall`, `city`.
* `<arena>` - (required) The combat arena to use if outdoor combat is initiated while the
party is on this space.
* `<ground>` - The ground type of this terrain.
* `<trim-for>` - The ground type of the terrain for which this terrain is a trim.
* `<editor>` - Some editor-specific settings. It supports the following subtags:
	* `<shortcut>` - The shortcut key to select this terrain (should be a letter).
	* `<object>` - Information about a large multi-space object, such as a pentacle or
	large pile of rubble - when you place one part of the object, the editor uses this
	information to place the remaining parts at the same time. It requires three subtags:
	`<num>`, which carries a unique integer identifying the object; `<pos>`, which is a
	point indicating the location of this terrain within the object's whole; and `<size>`,
	a point specifying the size of the object.

Item Types
----------

The _items.xml_ file stores the definitions of all the items in the scenario. The root
element is `<items>`, and the only toplevel tag is `<item>`, of which there can be any
number. The `<item>` element also has a required `id` attribute which must be unique. The
following tags are allowed in a `<item>` element:

* `<variety>` - (required) A string indicating the type of item this is. For a list of
valid values, search for `eItemType` in [estreams.cpp](../../src/classes/estreams.cpp).
* `<level>` - (required) The item level.
* `<awkward>` - The item's encumbrance level.
* `<bonus>` - The item's bonus.
* `<protection>` - The item's protection level.
* `<charges>` - The number of charges the item usually has.
* `<weapon-type>` - The key skill for the weapon (only if it's melee weapon, bow,
crossbow, or missile with no ammo). For a list of valid values, search for `eSkill` in
[estreams.cpp](../../src/classes/estreams.cpp).
* `<missile-type>` - The missile animation to use (only if it's actually a missile -
arrows, bolts, thrown missiles, or missiles with no ammo).
* `<pic>` - (required) The graphic to use.
* `<flag>` - The item's type flag.
* `<value>` - (required) The item's monetary value.
* `<weight>` - (required) The item's weight.
* `<class>` - The item's special class.
* `<name>` - The item's short (unidentified) name.
* `<full-name>` - The item's full (identified) name.
* `<treasire>` - The item's treasure class.
* `<ability>` - The item's special ability. This element requires three subtags:
	* `<type>` - The type of ability, as a string. For a list of valid values, search for
	`eItemAbil` in [estreams.cpp](../../src/classes/estreams.cpp).
	* `<strength>` - The item's ability strength.
	* `<data>` - The item's ability data.
	* `<use-flag>` - (optional) The item's magic use flag. Must be one of `harm-one`,
	`help-one`, `harm-all`, `help-all`.
* `<properties>` - Contains several boolean subtags specifying properties of this item.
Recognized subtags are `<identified>` (indicating it is _always_ identified), `<magic>`,
`<cursed>`, `<concealed>`, `<enchanted>`, `<unsellable>`. Note that the editor UI gives no
access to the `<enchanted>` flag.
* `<description>` - A description of the item. The scenario editor wraps the contents of
this element in a `CDATA` declaration.

Monster Types
-------------

The _monsters.xml_ file stores the definitions of all the monsters in the scenario. The
root element is `<monsters>`, and the only toplevel tag is `<monster>`, of which there can
be any number. The `<monster>` element also has a required `id` attribute which must be
unique. The following tags are allowed in a `<monster>` element:

* `<name>` - (required) The name of the monster.
* `<level>` - (required) The monster's level.
* `<armor>` - (required) The monster's armor rating.
* `<skill>` - (required) The monster's melee combat skill.
* `<hp>` - (required) The monster's maximum hit points.
* `<speed>` - (required) The monster's base action points.
* `<treasure>` - The treasure class the monster drops upon death.
* `<mage>` - The monster's mage spells skill.
* `<priest>` - The monster's priest spells skill.
* `<race>` - The monster's species. For a list of valid values, search for `eRace` in
[estreams.cpp](../../src/classes/estreams.cpp).
* `<abilities>` - The monster's special abilities.
* `<attacks>` - The monster's melee attacks. This can contain up to three `<attack>`
elements, the contents of which are a dice expression (eg `3d6`). The `<attack>` element
also requires a `type` attribute specifying the type of attack. For a list of valid
values, search for `eMonstMelee` in [estreams.cpp](../../src/classes/estreams.cpp).
* `<pic>` - (required) The monster's graphic.
* `<default-face>` - The monster's default talking portrait.
* `<onsight>` - A special node to call the first time the party sights a monster of this
type.
* `<voice>` - A sound to occasionally play when this monster is in sight.
* `<summon>` - The monster's summoning level.
* `<attitude>` - The monster's default attitude. Must be one of `friendly`, `docile`,
`hostile-a`, `hostile-b`.
* `<immunity>` - The monster's immunities. It can contain two boolean subtags (`<all>`,
indicating invulnerability, and `<fear>`, indicating the "mindless" ability) and four
integer subtags (`<fire>`, `<cold>`, `<poison>`, `<magic>`).
* `<loot>` - Specifies an item that the monster drops on death. The `<type>` subtags
specifies which item while the `<chance>` subtag specifies the likelihood of dropping it.

The `<abilities>` tag can have any of the following subtags:

* `<invisible>` - An empty element whose presence indicates that the monster is
permanently invisible.
* `<guard>` - An empty subtag whose presence indicates that the monster will seek out the
party if the town goes hostile.
* `<general>` - Requires the following attributes and subtags:
	* `type=` - Must be one of `dmg`, `dmg2`, `status`, `status2`, `field`, `petrify`,
	`drain-sp`, `drain-xp`, `steal-food`, `steal-gold`, `kill`, `stun`.
	* `<type>` - Must be one of `ray`, `touch`, `gaze`, `breath`, `spit`.
	* `<missile>` - (not if `<type>touch</type>`) The missile graphic.
	* `<strength>` - The ability strength.
	* `<range>` - (not if `<type>touch</touch>`) The ability range.
	* `<chance>` - Percentage chance of activating the ability. Tenths are supported (eg
	12.5).
	* `<extra>` - (only if `type="dmg"`, `type="dmg2"`) The type of damage to deal. Must
	be one of `weap`, `fire`, `poison`, `magic`, `unblockable`, `cold`, `undead`, `demon`,
	`spec`.
	* `<extra>` - (only if `type="status"`, `type="status2"`, `type="stun"`) The type of
	status effect to afflict. For a list of valid values, search for `eStatus` in
	[estreams.cpp](../../src/classes/estreams.cpp).
	* `<extra>` - (only if `type="field"`) The type of field to create. For a list of
	valid values, search for `eFieldType` in [estreams.cpp](../../src/classes/estreams.cpp),
	but note that `explored` and `spec-spot` are not allowed.
* `<missile>` - Requires the following attributes and subtags:
	* `type=` - Must be `missile`.
	* `<type>` - Must be one of `arrow`, `arrow++`, `dart`, `spear`, `stone`, `star`,
	`spine`, `knife`, `bolt`, `boulder`.
	* `<missile>` - The missile graphic.
	* `<strength>` - The ability strength.
	* `<skill>` - The monster's missile weapons skill.
	* `<range>` - The ability range.
	* `<chance>` - Percentage chance of activating the ability. Tenths are supported (eg
	12.5).
* `<summon>` - Requires the following attributes and subtags:
	* `type=` - Must be `summon`.
	* `<type>` - Must be one of `type`, `lvl`, `race`.
	* `<min>`, `<max>` - Specifies how many helpers to summon.
	* `<duration>` - How long the helpers will stay.
	* `<chance>` - Percentage chance of activating the ability. Tenths are supported (eg
	12.5).
* `<radiate>` - Requires the following attributes and subtags:
	* `type=` - Myst be `radiate`.
	* `<type>` - The type of field to radiate. For a list of valid values, search for
	`eFieldType` in [estreams.cpp](../../src/classes/estreams.cpp), but note that
	`explored` and `spec-spot` are not allowed.
	* `<chance>` - Percentage chance of activating the ability. Tenths are supported (eg
	12.5).
* `<special>` - Requires the following attributes and subtags:
	* `type=` - Must be one of `splits`, `martyr`, `absorb`, `old-web`, `old-heat`,
	`spec-act`, `spec-hit`, `spec-death`.
	* `<param>` - (optional, max 3) A parameter for the ability. The meaning of this
	depends on the value of the `type` attribute.

As mentioned, each of the above elements (except the first two) has a required `type`
attribute. There can only be one element with a given value of `type`.

Outdoors Header
---------------

The _out**X**~**Y**.xml_ files (where **X** and **Y** are substituted with the _x_ and _y_
coordinates of the section) store most of the data for a single outdoor sector. The root
element is `<sector>`, and the following toplevel tags are allowed:

* `<name>` - (required) The name of the outdoor sector.
* `<comment>` - A comment on the sector for the scenario designer (only seen in the
editor).
* `<sound>` - The ambient sound that is occasionally played while wandering through the
sector. May be an integer sound ID or one of the special values `birds` or `drip`.
* `<encounter>`, `<wandering>` - (max 4 of each) An outdoors encounter. These are
identical in structure; the only difference is that `<wandering>` encounters will
occasionally be spawned at the wandering encounter arrival points, while `<encounter>` is
spawned by a special node. Allows the following attributes and subtags:
	* `can-flee=` - A boolean indicating whether the encounter is permitted to flee if the
	party is too strong.
	* `<monster>` - (required, max 10) Specifies one of the monsters in this encounter.
	How many of this monster spawn is determined by the position of this `<monster>`
	element relative to the other `<monster>` elements. This supports a boolean `friendly`
	attribute.	You can have at most 3 with `friendly="true"` and at most 7 with
	`friendly="false"` (the	default).
	* `<onmeet>` - A special node to call when the encounter starts.
	* `<onwin>` - A special node to call when the encounter is beaten.
	* `<onflee>` - A special node to call when the party flees the encounter.
	* `<sdf>` - The SDF to prevent the encounter, as a point.
* `<sign>` - (max unbounded) The text of a particular sign. Requires an `id` attribute to
specify which sign it applies to. The scenario editor wraps the contents of this element
in a `CDATA` declaration.
* `<area>` - (max unbounded) The definition of an area description rect. This is a rect
type, but it also contains the area name as its content.
* `<string>` - (max unbounded) A string for use by outdoor special nodes. Requires an `id`
attribute to specify which string it is. The scenario editor wraps the contents of this
element in a `CDATA` declaration.

Town Header
-----------

The _town**N**.xml_ files (where **N** is substituted with number of the town) store most
of the data for a single town. The root element is `<town>`, and the following toplevel
tags are allowed:

* `<size>` - (required, must be first) The size of the town. Must be one of `32`, `48`,
`64`.
* `<name>` - (required) The town name.
* `<comment>` - (max 3) A comment on the town for the scenario designer (only seen in the
editor).
* `<bounds>` - (required) A rectangle indicating the usable area of the town - stepping on
the border of this rectangle will cause the party to leave town.
* `<difficulty>` - (required) An integer indicating the difficulty of this town.
* `<lighting>` - (required) The town's lighting type. Must be one of `lit`, `dark`,
`drains`, `none`.
* `<onenter>` - (max 2) A special node to call when the party enters this town. It can
have a `condition` attribute which can be either `alive` (the default) or `dead`.
* `<exit>` - (max 4) A point at which the party will end up at when exiting the town in
the direction indicated by the `dir` attribute (which can be one of `n`, `e`, `w`, `s`).
* `<onexit>` - (max 4) A special node to call when the party exists the town in the
direction indicated by the `dir` attribute.
* `<onoffend>` - A special node to call when the player makes the town hostile.
* `<timer>` - (max 8) A town timer.
* `<flags>` - (required) Several town flags, mostly booleans. Supports the following
subtags:
	* `<chop>` - Information about how the town dies, given in the attributes `day`,
	`event`, (which work together to determine if it dies) and `kills` (which is a
	separate check).
	* `<hidden>`
	* `<strong-barriers>`
	* `<defy-mapping>`
	* `<defy-scrying>`
* `<wandering>` - (max 4) Definition of a wandering monster group. It can contain up to 4
`<monster>` subtags specifying a monster to appear as part of the group. The first may
appear twice.
* `<sign>` - (max unbounded) The text of a particular sign. Requires an `id` attribute to
specify which sign it applies to. The scenario editor wraps the contents of this element
in a `CDATA` declaration.
* `<area>` - (max unbounded) The definition of an area description rect. This is a rect
type, but it also contains the area name as its content.
* `<string>` - (max unbounded) A string for use by town special nodes. Requires an `id`
attribute to specify which string it is. The scenario editor wraps the contents of this
element in a `CDATA` declaration.
* `<item>` - (max unbounded) The definition of a placed item. It requires an `id`
attribute specifying its position in the list of placed items, and allows the following
subtags:
	* `<type>` - (required) The type of item.
	* `<mod>` - A modifier for the item (for example, an enchantment)
	* `<charges>` - The number of charges.
	* `<always>` - A boolean indicating whether it always respawns.
	* `<property>` - A boolean indicating whether it belongs to someone other than the
	player.
	* `<contained>` - A boolean indicating whether it is in a container.
* `<creature>` - (max unbounded) The definition of a placed monster. It requires an `id`
attribute specifying its position in the list of placed monsters, and allows the following
subtags:
	* `<type>` - (required) The type of monster.
	* `<attitude>` - (required) The creature's starting attitude.
	* `<mobility>` - (required) The creature's mobility setting (currently 0 or 1).
	* `<sdf>` - (optional) An SDF to set when the creature dies, as a point.
	* `<encounter>` - (optional) The special encounter group number this creature belongs
	to. If it's 0, the creature doesn't belong to a special group.
	* `<time>` - (optional) When the monster appears. It requires a `type` attribute (one
	of `always`, `after-day`, `until-day`, `travel-a`, `travel-b`, `travel-c`,
	`after-event`, `until-event`, `after-chop`) and sometimes a `<day>` and/or `<event>`
	subtag.
	* `<face>` - The talking portrait to use for this creature.
	* `<personality>` - The talking personality to use for this creature.
	* `<onkill>` - A special node to call when this creature dies.
	* `<ontalk>` - A special node to call when attempting to talk to this creature.

(...more to come...)
