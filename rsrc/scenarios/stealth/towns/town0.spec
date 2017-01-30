@if-sdf = 0
	sdf 101, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto 2
@town-attitude = 1
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 2
	sdf 0, 0
	msg 1, 2, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 3
	sdf -1, -1
	msg 3, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 4
	sdf -1, -1
	msg 4, 5, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 5
	sdf 0, 1
	msg 6, -1, 1
	pic 130, 1
	ex1 34, 6, -1
	ex2 -1, -1, -1
	goto -1
@damage = 6
	sdf -1, -1
	msg 12, -1, -1
	pic 0, 4
	ex1 10, 10, -1
	ex2 0, 1, -1
	goto -1
@once-disp-msg = 7
	sdf 0, 2
	msg 13, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 8
	sdf 0, 3
	msg 14, 15, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 9
	sdf 0, 4
	msg 16, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf-eq = 10
	sdf 0, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 11, -1
	ex2 -1, -1, -1
	goto 12
@once-give-spec-item = 11
	sdf 0, 5
	msg 17, 18, -1
	pic 0, 4
	ex1 12, 0, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf-eq = 12
	sdf 0, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 13, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 13
	sdf -1, -1
	msg 19, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@button-generic = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 11, 5, -1
	ex2 2, 1, -1
	goto -1
