@disp-msg = 0
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 1
	sdf 207, 0
	msg 1, -1, 1
	pic 137, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 2
@once-out-encounter = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 3
	sdf 207, 1
	msg 7, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 4
	sdf 207, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 5
@make-out-wandering = 5
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 6
	sdf 207, 3
	msg 8, -1, 1
	pic 18, 4
	ex1 28, 7, -1
	ex2 -1, -1, -1
	goto -1
@hp = 7
	sdf -1, -1
	msg 14, 15, -1
	pic 0, 4
	ex1 30, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 8
	sdf 207, 4
	msg 16, -1, 1
	pic 18, 4
	ex1 28, 7, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 9
	sdf 207, 5
	msg 22, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 10
	sdf 207, 6
	msg 23, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
