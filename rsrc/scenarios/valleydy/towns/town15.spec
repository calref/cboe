@once-dlog = 0
	sdf 15, 0
	msg 0, -1, 1
	pic 170, 1
	ex1 29, 2, -1
	ex2 -1, -1, -1
	goto 1
@block-move = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@damage = 2
	sdf -1, -1
	msg 6, -1, -1
	pic 0, 4
	ex1 2, 6, -1
	ex2 2, 1, -1
	goto -1
@block-move = 3
	sdf -1, -1
	msg 7, 8, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@stair = 4
	sdf -1, -1
	msg 9, -1, -1
	pic 0, 4
	ex1 42, 34, -1
	ex2 13, -1, -1
	goto -1
@once-dlog = 5
	sdf 15, 2
	msg 15, -1, 1
	pic 63, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf-eq = 6
	sdf 15, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 7, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 7
	sdf 15, 3
	msg 21, 22, -1
	pic 0, 4
	ex1 1, 0, -1
	ex2 -1, -1, -1
	goto -1
