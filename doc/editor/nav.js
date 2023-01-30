/*

The navbar, to be included in all files.

*/

var main_pages = [
	{link:"About.html", text:"About"},
	{link:"Building.html", text:"Tutorial"},
	{link:"Editing.html", text:"Making and Editing"},
	{link:"Outdoors.html", text:"Outdoors"},
	{link:"Towns.html", text:"Towns"},
	{link:"Terrain.html", text:"Terrain Types"},
	{link:"Monsters.html", text:"Monster Types"},
	{link:"Items.html", text:"Items"},
	{link:"Advanced.html", text:"Advanced"},
	{link:"Specials.html", text:"Special Encounters"},
	{link:"Dialogue.html", text:"Dialogue"},
	{link:"Graphics.html", text:"Customizing Graphics"},
	{link:"Testing.html", text:"Testing and Distributing"},
];

var appendices = [
	{link:"Specials.html", text:"Special Encounter Node Types"},
	{link:"Items.html", text:"Item Ability Types"},
	{link:"Monsters.html", text:"Monster Ability Types"},
	{link:"Terrain.html", text:"Starting Terrain Types"},
	{link:"Sounds.html", text:"Blades Sound Effects"},
	{link:"Magic.html", text:"Spell and Alchemy Lists"},
	{link:"Examples.html", text:"Special Encounter Examples"},
];

var in_appendix = document.location.pathname.indexOf("appendix") >= 0;

var main_prefix = in_appendix ? "../" : "./";;
var appendix_prefix = in_appendix ? "./" : "appendix/";

document.write('\n<ol>\n');

for(var page of main_pages) {
	document.write(`<li><a href="${main_prefix}${page.link}">${page.text}</a></li>\n`);
}

document.write('</ol>\n<h2>Appendices</h2>\n');
document.write('<ol style=list-style-type:upper-alpha>\n');

for(var page of appendices) {
	document.write(`<li><a href="${appendix_prefix}${page.link}">${page.text}</a></li>\n`);
}

document.write('\n</ol>\n');
