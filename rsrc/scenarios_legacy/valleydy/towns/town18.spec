@once-dlog = 0
	sdf 18, 0
	msg 0, -1, 1
	pic 73, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 1
	sdf 18, 1
	msg 6, -1, 1
	pic 106, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 2
	sdf 18, 2
	msg 12, -1, 1
	pic 55, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 3
@once-town-encounter = 3
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-item = 4
	sdf 18, 4
	msg 18, -1, -1
	pic 0, 4
	ex1 186, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 5
	sdf 18, 3
	msg 19, -1, -1
	pic 2, 4
	ex1 70, 400, -1
	ex2 -1, 6, -1
	goto -1
@status = 6
	sdf -1, -1
	msg 25, -1, -1
	pic 0, 4
	ex1 8, 1, 2
	ex2 -1, -1, -1
	goto -1
@block-move = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@disp-msg = 8
	sdf -1, -1
	msg 26, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-boat = 99
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 7
