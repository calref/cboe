@once-dlog = 0
	sdf -1, -1
	msg 0, -1, 1
	pic 143, 1
	ex1 33, 1, -1
	ex2 -1, -1, -1
	goto -1
@change-time = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 600, -1, -1
	ex2 -1, -1, -1
	goto 2
@rest = 2
	sdf -1, -1
	msg 6, -1, -1
	pic 0, 4
	ex1 -1, 100, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 3
	sdf 0, 0
	msg 7, 8, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 4
	sdf -1, -1
	msg 9, -1, 0
	pic 93, 1
	ex1 2, 6, -1
	ex2 3, 5, -1
	goto -1
@block-move = 5
	sdf -1, -1
	msg 15, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@disp-msg = 6
	sdf -1, -1
	msg 16, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 13
@end-scen = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 10, -1
	ex2 -1, -1, -1
	goto 9
@block-move = 9
	sdf -1, -1
	msg 17, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 10
	sdf 0, 5
	msg 18, -1, 1
	pic 31, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 11
	sdf -1, -1
	msg 24, -1, 1
	pic 130, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 12
	sdf -1, -1
	msg 30, -1, 1
	pic 130, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 13
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 14, -1
	ex2 -1, -1, -1
	goto 7
@disp-msg = 14
	sdf -1, -1
	msg 36, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 7
@if-sdf = 15
	sdf 0, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 17, -1
	ex2 -1, -1, -1
	goto 16
@disp-msg = 16
	sdf -1, -1
	msg 37, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 17
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto 18
@once-give-spec-item = 18
	sdf 0, 4
	msg 38, 39, -1
	pic 0, 4
	ex1 0, 0, -1
	ex2 -1, -1, -1
	goto -1
