@once-dlog = 0
	sdf -1, -1
	msg 0, -1, 1
	pic 31, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 1
@once-dlog = 1
	sdf -1, -1
	msg 6, -1, 1
	pic 31, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 2
@out-move-party = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 28, 39, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 3
	sdf 200, 0
	msg 12, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 4
	sdf 200, 1
	msg 13, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-item = 5
	sdf -1, -1
	msg 14, -1, -1
	pic 0, 4
	ex1 84, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 6
	sdf 200, 2
	msg 15, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 7
	sdf 200, 3
	msg 16, -1, 1
	pic 73, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 8
	sdf 200, 4
	msg 22, -1, 1
	pic 31, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 9
	sdf 200, 5
	msg 28, -1, 1
	pic 45, 3
	ex1 22, 10, -1
	ex2 -1, -1, -1
	goto 11
@once-out-encounter = 10
	sdf -1, -1
	msg 34, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@disp-msg = 12
	sdf -1, -1
	msg 35, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
