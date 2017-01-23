@once-disp-msg = 0
	sdf 201, 0
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 1
	sdf 201, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 2
@once-dlog = 2
	sdf -1, -1
	msg 2, -1, 1
	pic 137, 1
	ex1 37, 4, -1
	ex2 -1, -1, -1
	goto 3
@disp-msg = 3
	sdf -1, -1
	msg 1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@select-pc = 4
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 3, -1
	ex2 -1, -1, -1
	goto 5
@xp = 5
	sdf -1, -1
	msg 8, 9, -1
	pic 0, 4
	ex1 50, 0, -1
	ex2 -1, -1, -1
	goto 3
@disp-msg = 6
	sdf -1, -1
	msg 10, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 7
	sdf 201, 3
	msg 11, -1, 1
	pic 71, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 8
@once-out-encounter = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-item = 9
	sdf -1, -1
	msg 17, -1, -1
	pic 0, 4
	ex1 308, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 10
	sdf -1, -1
	msg 18, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-item = 11
	sdf -1, -1
	msg 19, 20, -1
	pic 0, 4
	ex1 248, 30, -1
	ex2 45, -1, -1
	goto -1
@once-dlog = 12
	sdf 201, 2
	msg 21, -1, 1
	pic 65, 3
	ex1 8, 13, -1
	ex2 -1, -1, -1
	goto -1
@make-out-monst = 13
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@once-give-dlog = 15
	sdf 201, 4
	msg 27, -1, 7
	pic 140, 3
	ex1 -1, -1, -1
	ex2 -1, 16, -1
	goto -1
@disp-msg = 16
	sdf -1, -1
	msg 33, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 17
	sdf 201, 5
	msg 34, -1, -1
	pic 0, 4
	ex1 2, -1, -1
	ex2 -1, -1, -1
	goto -1
