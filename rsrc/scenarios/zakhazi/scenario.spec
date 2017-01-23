@disp-msg = 0
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@change-time = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 30, -1, -1
	ex2 -1, -1, -1
	goto 2
@disp-sm-msg = 2
	sdf -1, -1
	msg 1, 2, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@change-time = 3
	sdf -1, -1
	msg 3, 4, -1
	pic 0, 4
	ex1 400, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 4
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 11, 24, -1
	ex2 -1, -1, -1
	goto 3
@if-sdf-eq = 5
	sdf 100, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 6, -1
	ex2 -1, -1, -1
	goto -1
@if-day = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 21, 13, -1
	ex2 -1, -1, -1
	goto 9
@once-dlog = 7
	sdf -1, -1
	msg 5, -1, 1
	pic 31, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 8
@set-sdf = 8
	sdf 100, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-day = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 19, 10, -1
	ex2 -1, -1, -1
	goto 11
@once-disp-msg = 10
	sdf 100, 1
	msg 11, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-day = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 11, 12, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 12
	sdf 100, 2
	msg 12, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 13
	sdf 100, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 7
@disp-msg = 14
	sdf -1, -1
	msg 13, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 15
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 16, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 16
	sdf 110, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 17
@once-give-spec-item = 17
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 1, -1
	ex2 -1, -1, -1
	goto 18
@once-dlog = 18
	sdf -1, -1
	msg 22, -1, 0
	pic 62, 3
	ex1 3, 20, -1
	ex2 2, 19, -1
	goto -1
@disp-msg = 19
	sdf -1, -1
	msg 28, 29, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 20
	sdf -1, -1
	msg 30, -1, 0
	pic 62, 3
	ex1 3, 22, -1
	ex2 2, 21, -1
	goto -1
@once-give-spec-item = 21
	sdf -1, -1
	msg 37, 38, -1
	pic 0, 4
	ex1 11, 0, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 22
	sdf 8, 3
	msg 21, 40, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 23
@once-give-spec-item = 23
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 3, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 24
	sdf 204, 4
	msg 41, 42, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 25
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 26, -1
	ex2 -1, -1, -1
	goto 27
@gold = 26
	sdf -1, -1
	msg 43, -1, -1
	pic 0, 4
	ex1 2500, 0, -1
	ex2 -1, -1, -1
	goto 27
@if-spec-item = 27
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 28, -1
	ex2 -1, -1, -1
	goto 30
@gold = 28
	sdf -1, -1
	msg 44, -1, -1
	pic 0, 4
	ex1 1000, 0, -1
	ex2 -1, -1, -1
	goto 30
@end-scen = 29
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 30
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 12, 31, -1
	ex2 -1, -1, -1
	goto 29
@gold = 31
	sdf -1, -1
	msg 45, -1, -1
	pic 0, 4
	ex1 1000, 0, -1
	ex2 -1, -1, -1
	goto 29
