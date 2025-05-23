@once-dlog = 0
	sdf 205, 0
	msg 0, -1, 1
	pic 185, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 1
	sdf -1, -1
	msg 6, -1, 1
	pic 8, 4
	ex1 24, 2, -1
	ex2 -1, -1, -1
	goto 17
@out-move-party = 2
	sdf -1, -1
	msg 12, -1, -1
	pic 0, 4
	ex1 44, 42, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 3
	sdf -1, -1
	msg 13, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 4
@block-move = 4
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@once-disp-msg = 5
	sdf 205, 1
	msg 14, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 6
	sdf 205, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 7
	sdf 205, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 8, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 8
	sdf 205, 3
	msg 15, -1, 1
	pic 89, 3
	ex1 22, 11, -1
	ex2 41, 10, -1
	goto 9
@block-move = 9
	sdf -1, -1
	msg 21, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@gold = 10
	sdf -1, -1
	msg 22, -1, -1
	pic 0, 4
	ex1 500, 1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 11
	sdf -1, -1
	msg 23, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 12
	sdf -1, -1
	msg 24, -1, 1
	pic 241, 1
	ex1 8, 13, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 13
	sdf 205, 4
	msg 30, -1, -1
	pic 241, 1
	ex1 221, 700, -1
	ex2 -1, 14, -1
	goto -1
@disp-msg = 14
	sdf -1, -1
	msg 36, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once = 15
	sdf 205, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 12
@once-disp-msg = 16
	sdf 205, 5
	msg 37, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 17
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@block-move = 18
	sdf -1, -1
	msg 38, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
