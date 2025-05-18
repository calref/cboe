@if-sdf = 0
	sdf 5, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, 1, -1
	goto -1
@block-move = 1
	sdf -1, -1
	msg 0, 1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@statistic = 2
	sdf -1, -1
	msg 2, 3, -1
	pic 100, 4
	ex1 2, 1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 3
	sdf 5, 2
	msg 4, -1, 1
	pic 165, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf-eq = 4
	sdf 5, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 5, -1
	ex2 -1, -1, -1
	goto 6
@disp-msg = 5
	sdf -1, -1
	msg 10, 11, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 8, 7, -1
	ex2 -1, -1, -1
	goto 11
@once-give-spec-item = 7
	sdf 5, 1
	msg 12, 13, -1
	pic 0, 4
	ex1 8, 1, -1
	ex2 -1, -1, -1
	goto 8
@xp = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 20, 0, -1
	ex2 -1, -1, -1
	goto 9
@once-give-spec-item = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 5, 1, -1
	ex2 -1, -1, -1
	goto 10
@once-give-spec-item = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 7, 0, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 11
	sdf 5, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 12, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 12
	sdf -1, -1
	msg 14, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 13
	sdf 5, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, 16, -1
	goto 14
@if-sdf = 14
	sdf 5, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, 15, -1
	goto -1
@disp-msg = 15
	sdf -1, -1
	msg 15, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 16
	sdf 5, 0
	msg 16, 17, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 17
@once-give-spec-item = 17
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 5, 0, -1
	ex2 -1, -1, -1
	goto -1
