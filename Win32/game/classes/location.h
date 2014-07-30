#ifndef _LOCATION_H
	#define _LOCATION_H

struct location
{
	unsigned char x, y;

	/* functions */
	location() : x(0), y(0) { }
	location(int ix, int iy) { x = ix; y = iy; }
	location toGlobal();		/* convert local coordinates to global*/
	location toLocal();			/* convert global coordinates to local*/
	location randomShift();

	short countWalls() const;	/* count number of walls around */
	short handleLever();		/* call dialog and eventually pull lever */
	void crumbleWall();
	bool isDoor() const;

	void pickLock(short pcNum);
	void bashDoor(short pcNum);
};

#endif
