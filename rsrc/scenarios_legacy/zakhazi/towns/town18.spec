@once-disp-msg = 0
	sdf 18, 0
	msg 0, 1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 1
	sdf 18, 1
	msg 2, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 2
	sdf 18, 2
	msg 3, 4, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@lever-generic = 3
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 5, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 4
	sdf -1, -1
	msg 5, -1, -1
	pic 0, 4
	ex1 28, 5, -1
	ex2 130, 71, -1
	goto 7
@if-sdf = 5
	sdf 18, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 6, -1
	ex2 -1, -1, -1
	goto 4
@disp-msg = 6
	sdf -1, -1
	msg 6, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 7
	sdf 18, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@lever-generic = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 9, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 9
	sdf 18, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 6, -1
	ex2 -1, -1, -1
	goto 10
@set-sdf = 10
	sdf 18, 4
	msg 7, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 11
@change-ter = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 28, 42, -1
	ex2 71, -1, -1
	goto -1
@if-sdf = 12
	sdf 18, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 13, -1
	ex2 -1, -1, -1
	goto 14
@change-ter = 13
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 28, 5, -1
	ex2 71, -1, -1
	goto 14
@if-sdf = 14
	sdf 18, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 15, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 15
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 28, 42, -1
	ex2 71, -1, -1
	goto -1
@once-disp-msg = 16
	sdf 18, 5
	msg 8, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-town-encounter = 17
	sdf 18, 6
	msg 9, 10, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 18
@rect-change-ter = 18
	sdf 82, 100
	msg -1, -1, -1
	pic 0, 4
	ex1 33, 42, -1
	ex2 36, 46, -1
	goto 19
@status = 19
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 4, 1, 3
	ex2 -1, -1, -1
	goto -1
@once-trap = 20
	sdf 18, 7
	msg -1, -1, -1
	pic 27, 4
	ex1 3, 1, -1
	ex2 40, -1, -1
	goto -1
@disp-msg = 21
	sdf -1, -1
	msg 11, 12, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once = 22
	sdf 18, 8
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 23
@once-dlog = 23
	sdf -1, -1
	msg 13, -1, 1
	pic 154, 1
	ex1 37, 24, -1
	ex2 -1, -1, -1
	goto -1
@hp = 24
	sdf -1, -1
	msg 19, -1, -1
	pic 0, 4
	ex1 100, 1, -1
	ex2 -1, -1, -1
	goto -1
@if-context = 25
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 16, 108, 26
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 26
	sdf 18, 8
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 27
@xp = 27
	sdf -1, -1
	msg 20, 21, -1
	pic 0, 4
	ex1 10, 0, -1
	ex2 -1, -1, -1
	goto 28
@once-town-encounter = 28
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, -1, -1
	ex2 -1, -1, -1
	goto -1
