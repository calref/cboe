@once-disp-msg = 0
	sdf 13, 0
	msg 0, 1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 1
	sdf 101, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 3, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 2
	sdf 101, 7
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, 3, -1
	goto -1
@change-ter = 3
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 14, 10, -1
	ex2 148, -1, -1
	goto -1
@once-dlog = 4
	sdf -1, -1
	msg 2, -1, 1
	pic 161, 1
	ex1 51, 5, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 5
	sdf 101, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 9, -1
	ex2 -1, -1, -1
	goto 6
@set-sdf = 6
	sdf 101, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 7
@once-dlog = 7
	sdf -1, -1
	msg 8, -1, 1
	pic 161, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 8
@town-visible = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 15, 1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 9
	sdf 101, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 10, -1
	ex2 -1, -1, -1
	goto 12
@once-dlog = 10
	sdf -1, -1
	msg 14, -1, 1
	pic 161, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 11
@set-sdf = 11
	sdf 101, 9
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 12
	sdf 101, 7
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 13, -1
	ex2 -1, -1, -1
	goto 14
@once-dlog = 13
	sdf -1, -1
	msg 20, -1, 1
	pic 161, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 14
	sdf 101, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 16, -1
	ex2 -1, -1, -1
	goto 7
@disp-msg = 15
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 16
	sdf -1, -1
	msg 26, -1, 0
	pic 161, 1
	ex1 3, 17, -1
	ex2 2, 18, -1
	goto -1
@disp-msg = 17
	sdf -1, -1
	msg 32, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 18
	sdf 101, 7
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 19
@once-dlog = 19
	sdf -1, -1
	msg 33, -1, 1
	pic 161, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@button-generic = 20
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 27, 21, -1
	ex2 12, 0, -1
	goto -1
