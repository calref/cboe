/*

The navbar, to be included in all files.

*/

document.write('\
<ol>\
<li><a href="About.html">About</a></li>\
<li><a href="Building.html">Tutorial</a></li>\
<li><a href="Editing.html">Making and Editing</a></li>\
<li><a href="Outdoors.html">Outdoors</a></li>\
<li><a href="Towns.html">Towns</a></li>\
<li><a href="Terrain.html">Terrain Types</a></li>\
<li><a href="Monsters.html">Monster Types</a></li>\
<li><a href="Items.html">Items</a></li>\
<li><a href="Advanced.html">Advanced</a></li>\
<li><a href="Specials.html">Special Encounters</a></li>\
<li><a href="Dialogue.html">Dialogue</a></li>\
<li><a href="Graphics.html">Customizing Graphics</a></li>\
<li><a href="Testing.html">Testing and Distributing</a></li>\
</ol>\
<h2>Appendices</h2>\
<ol style="list-style-type:upper-alpha">\
<li><a href="appendix/Specials.html">Special Encounter Node Types</a></li>\
<li><a href="appendix/Items.html">Item Ability Types</a></li>\
<li><a href="appendix/Monsters.html">Monster Ability Types</a></li>\
<li><a href="appendix/Terrain.html">Starting Terrain Types</a></li>\
<li><a href="appendix/Sounds.html">Blades Sound Effects</a></li>\
<li><a href="appendix/Messages.html">Text Messages</a></li>\
<li><a href="appendix/Magic.html">Spell and Alchemy Lists</a></li>\
<li><a href="appendix/Examples.html">Special Encounter Examples</a></li>\
</ol>\
');

if(document.location.pathname.contains("appendix")) {
	var links = document.querySelectorAll(".navbar a");
	console.log(links);
	for(var i = 0; i < links.length; i++) {
		links[i].href = links[i].href.replace("appendix/","");
	}
}
