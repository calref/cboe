@disp-msg = 0
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 1
	sdf -1, -1
	msg 1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 2
@once-give-item = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 383, 50, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 3
	sdf 202, 0
	msg 2, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 4
	sdf 202, 1
	msg 3, -1, -1
	pic 78, 1
	ex1 364, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 5
	sdf 202, 2
	msg 9, -1, -1
	pic 80, 1
	ex1 383, -1, -1
	ex2 -1, -1, -1
	goto -1
