@once-dlog = 0
	sdf 203, 0
	msg 0, -1, 1
	pic 31, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 1
	sdf 203, 1
	msg 6, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 2
	sdf 203, 2
	msg 7, 8, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 3
	sdf 203, 3
	msg 9, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 4
	sdf -1, -1
	msg 10, -1, 1
	pic 19, 4
	ex1 24, 5, -1
	ex2 -1, -1, -1
	goto -1
@change-time = 5
	sdf -1, -1
	msg 16, 17, -1
	pic 0, 4
	ex1 900, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 6
	sdf 203, 4
	msg 18, 19, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-item = 7
	sdf -1, -1
	msg 20, 21, -1
	pic 0, 4
	ex1 254, 400, -1
	ex2 100, -1, -1
	goto 8
@once-give-item = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 259, -1, -1
	ex2 -1, -1, -1
	goto 9
@once-give-item = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 258, -1, -1
	ex2 -1, -1, -1
	goto -1
