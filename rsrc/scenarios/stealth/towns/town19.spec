@alchemy = 0
	sdf -1, -1
	msg 0, 1, -1
	pic 0, 4
	ex1 7, -1, -1
	ex2 -1, -1, -1
	goto 1
@alchemy = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 8, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 2
	sdf 19, 0
	msg 2, 3, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 3
	sdf 17, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 4, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 4
	sdf 19, 1
	msg 4, -1, -1
	pic 138, 1
	ex1 387, 1000, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 5
	sdf -1, -1
	msg 10, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 6
	sdf -1, -1
	msg 11, -1, 1
	pic 1004, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
