@once-dlog = 0
	sdf 10, 0
	msg 0, -1, 1
	pic 149, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 1
@once-give-spec-item = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 6, 0, -1
	ex2 -1, -1, -1
	goto 2
@set-sdf = 2
	sdf 5, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 15
@stair = 3
	sdf -1, -1
	msg 6, -1, -1
	pic 0, 4
	ex1 36, 61, -1
	ex2 9, -1, -1
	goto -1
@if-ter = 4
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 9, 20, -1
	ex2 154, 5, -1
	goto -1
@once-dlog = 5
	sdf -1, -1
	msg 12, -1, 1
	pic 14, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 6
@change-ter = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 9, 20, -1
	ex2 170, -1, -1
	goto 16
@disp-msg = 7
	sdf -1, -1
	msg 24, 25, -1
	pic 138, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto 11
@once-dlog = 8
	sdf -1, -1
	msg 26, -1, 1
	pic 143, 1
	ex1 33, 9, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 9
	sdf -1, -1
	msg 32, 33, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 10
@death = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto -1
@death = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 12
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@disp-msg = 13
	sdf -1, -1
	msg 34, 35, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 14
	sdf 10, 1
	msg 36, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@xp = 15
	sdf -1, -1
	msg 37, -1, -1
	pic 0, 4
	ex1 30, 0, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 16
	sdf 10, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 17
	sdf 10, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 18, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 9, 20, -1
	ex2 170, -1, -1
	goto -1
