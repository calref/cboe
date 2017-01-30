@once-dlog = 0
	sdf 7, 0
	msg 0, -1, 1
	pic 189, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 1
	sdf -1, -1
	msg 6, 7, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 2
	sdf 7, 1
	msg 8, 9, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 3
	sdf 7, 2
	msg 10, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once = 4
	sdf 7, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 5
@once-dlog = 5
	sdf -1, -1
	msg 11, -1, 1
	pic 154, 1
	ex1 37, 6, -1
	ex2 40, 8, -1
	goto -1
@select-pc = 6
	sdf -1, -1
	msg 17, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto 7
@statistic = 7
	sdf -1, -1
	msg 18, -1, -1
	pic 100, 4
	ex1 2, 1, -1
	ex2 0, -1, -1
	goto -1
@statistic = 8
	sdf -1, -1
	msg 19, -1, -1
	pic 100, 4
	ex1 2, 1, -1
	ex2 0, -1, -1
	goto -1
@if-context = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 16, 108, 10
	ex2 -1, -1, -1
	goto -1
@if-sdf = 10
	sdf 7, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 11, -1
	ex2 -1, -1, -1
	goto 12
@disp-msg = 11
	sdf -1, -1
	msg 20, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 12
	sdf 7, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 13
@xp = 13
	sdf -1, -1
	msg 21, -1, -1
	pic 0, 4
	ex1 25, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 14
	sdf -1, -1
	msg 22, -1, 1
	pic 244, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 15
	sdf -1, -1
	msg 28, -1, 1
	pic 105, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 16
	sdf -1, -1
	msg 34, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@lever = 17
	sdf -1, -1
	msg 35, -1, -1
	pic 213, 1
	ex1 -1, 18, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 18
	sdf -1, -1
	msg 41, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@lever-generic = 19
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 20, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 20
	sdf -1, -1
	msg 42, -1, -1
	pic 0, 4
	ex1 9, 13, -1
	ex2 130, 131, -1
	goto -1
@lever-generic = 21
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 22, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 22
	sdf -1, -1
	msg 43, -1, -1
	pic 0, 4
	ex1 19, 26, -1
	ex2 130, 131, -1
	goto -1
@disp-msg = 23
	sdf -1, -1
	msg 44, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-trap = 24
	sdf 7, 4
	msg -1, -1, -1
	pic 27, 4
	ex1 1, 3, -1
	ex2 80, -1, -1
	goto -1
@if-sdf = 25
	sdf 8, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 26, -1
	ex2 -1, -1, -1
	goto 27
@disp-msg = 26
	sdf -1, -1
	msg 45, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@button-generic = 27
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 46, 46, -1
	ex2 8, 1, -1
	goto -1
