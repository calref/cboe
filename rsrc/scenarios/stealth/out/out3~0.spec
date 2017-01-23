@once-disp-msg = 0
	sdf 203, 0
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 1
	sdf 203, 0
	msg 1, -1, 1
	pic 195, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 2
	sdf -1, -1
	msg 7, -1, 1
	pic 190, 1
	ex1 47, 5, -1
	ex2 -1, -1, -1
	goto 3
@block-move = 3
	sdf -1, -1
	msg 13, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 4
	sdf -1, -1
	msg 14, -1, 1
	pic 17, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 7
@if-sdf = 5
	sdf 203, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 6, -1
	ex2 -1, -1, -1
	goto 4
@disp-msg = 6
	sdf -1, -1
	msg 20, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 7
	sdf 203, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 8
	sdf 203, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 9
@once-dlog = 9
	sdf -1, -1
	msg 21, -1, 0
	pic 16, 3
	ex1 3, 12, -1
	ex2 2, 10, -1
	goto -1
@disp-msg = 10
	sdf -1, -1
	msg 27, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 11
@death = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 12
	sdf -1, -1
	msg 28, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 13
	sdf -1, -1
	msg 29, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 14
	sdf 203, 4
	msg 30, -1, 1
	pic 16, 3
	ex1 29, 16, -1
	ex2 -1, -1, -1
	goto 15
@block-move = 15
	sdf -1, -1
	msg 36, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 16
	sdf -1, -1
	msg 37, -1, 0
	pic 16, 3
	ex1 2, 19, -1
	ex2 3, 17, -1
	goto -1
@disp-msg = 17
	sdf -1, -1
	msg 43, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 18
@death = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 19
	sdf -1, -1
	msg 44, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 20
	sdf 203, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 21, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 21
	sdf -1, -1
	msg 45, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-disp-msg = 22
	sdf 203, 5
	msg 46, 47, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 23
	sdf -1, -1
	msg 54, -1, 0
	pic 66, 1
	ex1 3, 24, -1
	ex2 2, 25, -1
	goto -1
@block-move = 24
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 25
	sdf -1, -1
	msg 48, -1, 1
	pic 17, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 26
@end-scen = 26
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
