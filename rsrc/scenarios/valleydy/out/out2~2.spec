@disp-msg = 0
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, 0, -1
	ex2 -1, 0, -1
	goto 1
@block-move = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 0, -1
	ex2 0, 0, -1
	goto -1
@once-give-dlog = 2
	sdf 208, 0
	msg 1, -1, -1
	pic 179, 1
	ex1 58, 0, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 3
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 0, -1
	ex2 1, 0, -1
	goto -1
@once-out-encounter = 4
	sdf 208, 1
	msg 7, -1, -1
	pic 0, 4
	ex1 0, 0, -1
	ex2 -1, 0, -1
	goto -1
@gold = 5
	sdf -1, -1
	msg 8, -1, -1
	pic 0, 4
	ex1 50, 0, -1
	ex2 -1, 0, -1
	goto -1
@once-dlog = 6
	sdf 208, 2
	msg 9, -1, 1
	pic 29, 4
	ex1 -1, 0, -1
	ex2 -1, 0, -1
	goto -1
