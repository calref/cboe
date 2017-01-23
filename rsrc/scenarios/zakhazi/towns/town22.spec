@once-dlog = 0
	sdf 22, 0
	msg 0, -1, 1
	pic 45, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 1
@once-give-spec-item = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 2
	sdf -1, -1
	msg 6, 7, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 3
	sdf -1, -1
	msg 8, -1, 0
	pic 31, 4
	ex1 9, 4, -1
	ex2 38, -1, -1
	goto -1
@once-dlog = 4
	sdf -1, -1
	msg 14, -1, 1
	pic 31, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 5
@gold = 5
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 500, 0, -1
	ex2 -1, -1, -1
	goto 6
@call-global = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 25
