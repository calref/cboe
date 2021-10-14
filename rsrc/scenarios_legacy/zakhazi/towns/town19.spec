@disp-msg = 0
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-town-encounter = 1
	sdf 19, 4
	msg 1, 2, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-ter = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 15, 24, -1
	ex2 154, 3, -1
	goto -1
@if-sdf = 3
	sdf 17, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 7, -1
	ex2 -1, -1, -1
	goto 9
@once-dlog = 4
	sdf -1, -1
	msg 3, -1, 1
	pic 80, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 5
@set-sdf = 5
	sdf 19, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 6
@change-ter = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 15, 24, -1
	ex2 156, -1, -1
	goto -1
@if-sdf = 7
	sdf 19, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 8, -1
	ex2 -1, -1, -1
	goto 4
@change-ter = 8
	sdf -1, -1
	msg 9, -1, -1
	pic 0, 4
	ex1 15, 24, -1
	ex2 156, -1, -1
	goto -1
@once-dlog = 9
	sdf -1, -1
	msg 10, -1, 1
	pic 80, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 10
	sdf 19, 6
	msg 16, 17, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 26, 38, -1
	ex2 139, -1, -1
	goto 12
@rect-place-field = 12
	sdf 100, 15
	msg -1, -1, -1
	pic 0, 4
	ex1 38, 27, -1
	ex2 38, 27, -1
	goto -1
@once-dlog = 13
	sdf -1, -1
	msg 18, -1, 1
	pic 22, 4
	ex1 8, 14, -1
	ex2 -1, -1, -1
	goto 15
@block-move = 14
	sdf -1, -1
	msg 24, 25, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@block-move = 15
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@change-ter = 16
	sdf -1, -1
	msg 26, -1, -1
	pic 0, 4
	ex1 26, 38, -1
	ex2 148, -1, -1
	goto -1
