@once-disp-msg = 0
	sdf 3, 0
	msg 0, 1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 1
	sdf 3, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 2, -1
	ex2 -1, -1, -1
	goto 3
@disp-msg = 2
	sdf -1, -1
	msg 2, 5, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 3
	sdf 3, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 4, -1
	ex2 -1, -1, -1
	goto 10
@if-spec-item = 4
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 5, 5, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 5
	sdf -1, -1
	msg 3, 4, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 6
@once-give-spec-item = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 5, 1, -1
	ex2 -1, -1, -1
	goto 7
@once-give-item = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 284, -1, -1
	ex2 -1, 8, -1
	goto 9
@set-sdf = 9
	sdf 3, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 10
	sdf 3, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 11
	sdf 3, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
