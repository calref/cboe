@once-dlog = 0
	sdf 208, 1
	msg 0, -1, 1
	pic 88, 3
	ex1 8, 2, -1
	ex2 -1, -1, -1
	goto 1
@block-move = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@once-out-encounter = 2
	sdf -1, -1
	msg 6, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-item = 3
	sdf -1, -1
	msg 7, -1, -1
	pic 0, 4
	ex1 175, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 4
	sdf 208, 3
	msg 8, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 5
	sdf 208, 2
	msg 9, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 6
	sdf 208, 4
	msg 10, 11, -1
	pic 0, 4
	ex1 2, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 7
	sdf 208, 0
	msg 12, -1, 1
	pic 8, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 8
	sdf 208, 5
	msg 18, -1, 1
	pic 18, 4
	ex1 28, 9, -1
	ex2 -1, -1, -1
	goto -1
@status = 9
	sdf -1, -1
	msg 24, -1, -1
	pic 0, 4
	ex1 3, 1, 9
	ex2 -1, -1, -1
	goto 10
@sp = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 100, 0, 0
	ex2 -1, -1, -1
	goto -1
