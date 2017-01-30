@once-dlog = 0
	sdf -1, -1
	msg 0, -1, 1
	pic 190, 1
	ex1 29, 1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 1
	sdf -1, -1
	msg 6, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 2
	sdf -1, -1
	msg 7, -1, 1
	pic 190, 1
	ex1 29, 4, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 3
	sdf -1, -1
	msg 13, -1, 1
	pic 1, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 5
@if-sdf = 4
	sdf 210, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 6, -1
	ex2 -1, -1, -1
	goto 3
@set-sdf = 5
	sdf 210, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 6
	sdf -1, -1
	msg 19, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 7
	sdf -1, -1
	msg 20, -1, 1
	pic 190, 1
	ex1 29, 8, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 8
	sdf -1, -1
	msg 26, 27, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 9
	sdf -1, -1
	msg 28, -1, 1
	pic 190, 1
	ex1 29, 10, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 10
	sdf 210, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 13, -1
	ex2 -1, -1, -1
	goto 11
@once-dlog = 11
	sdf -1, -1
	msg 34, -1, 1
	pic 7, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 12
@set-sdf = 12
	sdf 210, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 13
	sdf 206, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 15, -1
	ex2 -1, -1, -1
	goto 14
@disp-msg = 14
	sdf -1, -1
	msg 40, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 15
	sdf 210, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 16, -1
	ex2 -1, -1, -1
	goto 17
@food = 16
	sdf -1, -1
	msg 41, -1, -1
	pic 0, 4
	ex1 3, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 17
	sdf 210, 3
	msg 42, -1, 1
	pic 7, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 18
@once-give-item = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 303, -1, -1
	ex2 50, -1, -1
	goto -1
@if-sdf = 19
	sdf 210, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 20, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 20
	sdf -1, -1
	msg 48, 49, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 21
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@once-give-dlog = 22
	sdf 210, 4
	msg 50, -1, -1
	pic 2, 4
	ex1 363, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 23
	sdf 210, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 24
	sdf 210, 5
	msg 56, -1, -1
	pic 2, 4
	ex1 80, -1, -1
	ex2 -1, -1, -1
	goto -1
