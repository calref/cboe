@disp-msg = 0
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@disp-msg = 2
	sdf -1, -1
	msg 1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 3
@damage = 3
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 0, 4, -1
	goto -1
@once-disp-msg = 4
	sdf 1, 3
	msg 2, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-trap = 5
	sdf 1, 4
	msg -1, -1, -1
	pic 27, 4
	ex1 2, 1, -1
	ex2 20, -1, -1
	goto -1
@once-disp-msg = 6
	sdf 1, 0
	msg 3, 4, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-trap = 7
	sdf 1, 1
	msg 5, -1, -1
	pic 27, 4
	ex1 4, 0, -1
	ex2 80, -1, -1
	goto -1
@once-give-dlog = 8
	sdf 1, 2
	msg 6, -1, 0
	pic 132, 1
	ex1 -1, -1, -1
	ex2 -1, 9, -1
	goto -1
@disp-msg = 9
	sdf -1, -1
	msg 12, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 10
	sdf 199, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 11, -1
	ex2 -1, -1, -1
	goto 12
@disp-msg = 11
	sdf -1, -1
	msg 13, 14, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 12
	sdf 199, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 13, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 13
	sdf -1, -1
	msg 15, 16, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 14
	sdf 1, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 15
@once-dlog = 15
	sdf -1, -1
	msg 17, -1, 1
	pic 189, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 16
	sdf 1, 9
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 17, -1
	ex2 -1, -1, -1
	goto 14
@once-dlog = 17
	sdf -1, -1
	msg 23, -1, 1
	pic 30, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 18
@once-dlog = 18
	sdf -1, -1
	msg 29, -1, 1
	pic 30, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 19
@gold = 19
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2500, 0, -1
	ex2 -1, -1, -1
	goto 20
@once-dlog = 20
	sdf -1, -1
	msg 35, -1, 1
	pic 4, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 21
@xp = 21
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 100, 0, -1
	ex2 -1, -1, -1
	goto 22
@end-scen = 22
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
