/*
	Gathers <dt> elements and builds a list of links to them.
*/

document.addEventListener('DOMContentLoaded', function() {
	var elems = document.querySelectorAll('.autolink > dt');
	var links = []
	for(var i = 0; i < elems.length; i++) {
		var li = document.createElement('li');
		links.push(li);
		var name = elems[i].innerHTML;
		name = name.split(/\s*[:-]\s+/);
		name = name[1] || name[0]
		li.innerHTML = name;
		li.addEventListener('click', (function(n) {
			return function() {
				//var newloc = Object.create(location)
				//newloc.search = "jumpto=" + n;
				history.pushState(null, null, "?jumpto=" + n);
				elems[n].scrollIntoView(true);
			};
		})(i));
	}
	links.sort(function(a,b) {
		if(a.innerHTML < b.innerHTML)
			return -1;
		if(a.innerHTML > b.innerHTML)
			return 1;
		return 0;
	});
	document.querySelector('.navbar').insertAdjacentHTML('beforeend',"<h2>Jump to:</h2>");
	var nav = document.createElement('ul');
	nav.classList.add('nav-jump');
	for(var i = 0; i < links.length; i++)
		nav.appendChild(links[i]);
	document.querySelector('.navbar').appendChild(nav);
	// Now check if one was requested in the query string
	var dest = location.search.match(/^jumpto\=(\d)/, '');
	if(dest && dest[1] >= 0 && dest[1] < elems.length)
		elems[dest].scrollIntoView();
});
