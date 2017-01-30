@block-move = 0
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto 1
@once-dlog = 1
	sdf -1, -1
	msg 0, -1, 1
	pic 190, 1
	ex1 29, 3, -1
	ex2 -1, -1, -1
	goto 2
@disp-msg = 2
	sdf -1, -1
	msg 6, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 3
	sdf -1, -1
	msg 7, -1, 0
	pic 25, 3
	ex1 3, -1, -1
	ex2 2, 4, -1
	goto -1
@start-shop = 4
	sdf -1, -1
	msg 13, -1, -1
	pic 0, 4
	ex1 18, 0, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 5
	sdf -1, -1
	msg 14, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 6
@gold = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 200, 0, -1
	ex2 -1, -1, -1
	goto -1
@once = 7
	sdf 200, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 8
@once-dlog = 8
	sdf -1, -1
	msg 15, -1, 1
	pic 194, 1
	ex1 8, 10, -1
	ex2 -1, -1, -1
	goto 9
@block-move = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-give-dlog = 10
	sdf 200, 0
	msg 21, -1, 1
	pic 154, 1
	ex1 321, -1, -1
	ex2 -1, 11, -1
	goto -1
@make-out-monst = 11
	sdf -1, -1
	msg 27, 28, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 12
	sdf 200, 1
	msg 29, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 13
	sdf 200, 2
	msg 30, 31, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@disp-msg = 15
	sdf -1, -1
	msg 32, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 16
	sdf 200, 3
	msg 33, -1, 1
	pic 65, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
