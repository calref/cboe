@once-trap = 0
	sdf 5, 0
	msg -1, -1, -1
	pic 27, 4
	ex1 0, 2, -1
	ex2 25, -1, -1
	goto -1
@once-trap = 1
	sdf 5, 1
	msg 0, -1, -1
	pic 27, 4
	ex1 8, 0, -1
	ex2 30, -1, -1
	goto -1
@once-disp-msg = 2
	sdf 5, 2
	msg 1, 2, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 3
	sdf 1, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 4, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 4
	sdf 100, 8
	msg 3, 4, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 5
@once-give-spec-item = 5
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 13, 0, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 6
	sdf 101, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 7, -1
	ex2 -1, -1, -1
	goto 8
@town-attitude = 7
	sdf -1, -1
	msg 5, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 8
	sdf 5, 3
	msg 6, 7, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
