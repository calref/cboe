@block-move = 0
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto 1
@once-dlog = 1
	sdf -1, -1
	msg 0, -1, 0
	pic 30, 4
	ex1 3, -1, -1
	ex2 2, 2, -1
	goto -1
@if-sdf = 2
	sdf 100, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 5, -1
	ex2 -1, -1, -1
	goto 3
@disp-msg = 3
	sdf -1, -1
	msg 6, 7, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 4
@end-scen = 4
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 5
	sdf -1, -1
	msg 8, -1, 1
	pic 16, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 6
@once-give-item = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 78, 1000, -1
	ex2 -1, -1, -1
	goto 7
@xp = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 20, 0, -1
	ex2 -1, -1, -1
	goto 8
@end-scen = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 9
	sdf 200, 0
	msg 14, -1, -1
	pic 81, 1
	ex1 383, -1, -1
	ex2 40, -1, -1
	goto -1
@disp-msg = 10
	sdf -1, -1
	msg 20, 21, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 11
	sdf 200, 2
	msg 22, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 12
	sdf 200, 1
	msg 23, 24, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 13
	sdf -1, -1
	msg 25, -1, 1
	pic 81, 1
	ex1 29, 14, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 14
	sdf -1, -1
	msg 31, 32, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
