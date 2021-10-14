@once = 0
	sdf 211, 0
	msg -1, 1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 1
@once-dlog = 1
	sdf -1, -1
	msg 0, -1, 1
	pic 190, 1
	ex1 8, 2, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 2
	sdf -1, -1
	msg 6, -1, 1
	pic 28, 3
	ex1 22, 3, -1
	ex2 38, 9, -1
	goto -1
@once-set-sdf = 3
	sdf 211, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 4
@disp-msg = 4
	sdf -1, -1
	msg 12, 13, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 5
@start-timer-scen = 5
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 50, 1, -1
	ex2 -1, -1, -1
	goto 8
@once-give-item = 8
	sdf -1, -1
	msg 14, -1, -1
	pic 0, 4
	ex1 370, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 9
	sdf -1, -1
	msg 15, -1, 1
	pic 28, 3
	ex1 7, 11, -1
	ex2 -1, -1, -1
	goto 10
@block-move = 10
	sdf -1, -1
	msg 21, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@start-shop = 11
	sdf -1, -1
	msg 22, -1, -1
	pic 0, 4
	ex1 35, 5, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 12
	sdf 211, 1
	msg 23, -1, 1
	pic 78, 1
	ex1 8, 13, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 13
	sdf -1, -1
	msg 36, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 14
	sdf 211, 2
	msg 29, -1, 1
	pic 78, 1
	ex1 8, 15, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 15
	sdf -1, -1
	msg 35, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 16
	sdf 211, 3
	msg 37, 38, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
