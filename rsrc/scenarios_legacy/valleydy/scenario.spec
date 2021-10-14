@if-sdf-eq = 0
	sdf 2, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto 3
@disp-msg = 1
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@if-spec-item = 3
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 4, 4, -1
	ex2 -1, -1, -1
	goto 5
@disp-msg = 4
	sdf -1, -1
	msg 1, 2, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 5
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 3, 6, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 6
	sdf -1, -1
	msg 3, 4, -1
	pic 0, 4
	ex1 3, 1, -1
	ex2 -1, -1, -1
	goto 7
@gold = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 400, 0, -1
	ex2 -1, -1, -1
	goto -1
@status = 8
	sdf -1, -1
	msg 5, -1, -1
	pic 0, 4
	ex1 1, 0, 7
	ex2 -1, -1, -1
	goto -1
@if-context = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 0, -1
	ex2 -1, -1, -1
	goto 10
@rect-swap-ter = 10
	sdf 252, 253
	msg 7, -1, -1
	pic 0, 4
	ex1 0, 0, -1
	ex2 63, 63, -1
	goto -1
@status = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 4, 1, 7
	ex2 -1, -1, -1
	goto 15
@once-disp-msg = 12
	sdf 100, 6
	msg 8, 9, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 13
	sdf -1, -1
	msg 10, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@disp-msg = 14
	sdf -1, -1
	msg 11, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-sm-msg = 15
	sdf -1, -1
	msg 12, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 12
@town-attitude = 21
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, -1, -1
	goto -1
