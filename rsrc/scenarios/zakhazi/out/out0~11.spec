@if-sdf = 0
	sdf 211, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto 2
@disp-msg = 1
	sdf -1, -1
	msg 0, 1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 2
	sdf -1, -1
	msg 2, 3, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 3
	sdf -1, -1
	msg 4, -1, 0
	pic 241, 1
	ex1 2, 5, -1
	ex2 3, 4, -1
	goto -1
@disp-msg = 4
	sdf -1, -1
	msg 10, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 5
	sdf 211, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 6, -1
	ex2 -1, -1, -1
	goto 7
@disp-msg = 6
	sdf -1, -1
	msg 11, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 7
	sdf 211, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 8
@change-time = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2500, -1, -1
	ex2 -1, -1, -1
	goto 9
@gold = 9
	sdf -1, -1
	msg 12, -1, -1
	pic 0, 4
	ex1 600, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 10
	sdf -1, -1
	msg 4, -1, 0
	pic 240, 1
	ex1 2, 11, -1
	ex2 3, 4, -1
	goto -1
@if-sdf = 11
	sdf 211, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 6, -1
	ex2 -1, -1, -1
	goto 16
@set-sdf = 12
	sdf 211, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 6, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 13
	sdf 211, 3
	msg 13, -1, 1
	pic 144, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 14
	sdf 211, 6
	msg 19, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 15
	sdf 211, 7
	msg 20, -1, 1
	pic 31, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 16
	sdf 211, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 8
