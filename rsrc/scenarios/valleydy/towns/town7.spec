@once-give-dlog = 0
	sdf 7, 0
	msg 0, -1, -1
	pic 53, 3
	ex1 345, -1, -1
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
@disp-msg = 3
	sdf -1, -1
	msg 10, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 4
	sdf -1, -1
	msg 11, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 5
	sdf -1, -1
	msg 12, -1, 1
	pic 135, 1
	ex1 34, 6, -1
	ex2 -1, -1, -1
	goto -1
@if-statistic = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 5, 8, -1
	ex2 11, 0, -1
	goto 7
@disp-msg = 7
	sdf -1, -1
	msg 18, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@alchemy = 8
	sdf -1, -1
	msg 19, 20, -1
	pic 0, 4
	ex1 3, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 9
	sdf 7, 3
	msg 21, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 10
	sdf 7, 4
	msg 22, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@damage = 11
	sdf -1, -1
	msg 23, -1, -1
	pic 0, 4
	ex1 10, 10, -1
	ex2 0, 0, -1
	goto 12
@block-move = 12
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@disp-msg = 13
	sdf -1, -1
	msg 24, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 14
	sdf -1, -1
	msg 25, -1, 1
	pic 11, 4
	ex1 36, 15, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 15
	sdf -1, -1
	msg 31, -1, -1
	pic 0, 4
	ex1 1, 17, -1
	ex2 130, 131, -1
	goto -1
@if-sdf = 16
	sdf 7, 5
	msg -1, 1, -1
	pic 0, 4
	ex1 1, 17, -1
	ex2 -1, -1, -1
	goto 18
@disp-msg = 17
	sdf -1, -1
	msg 32, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 18
	sdf -1, -1
	msg 33, -1, 1
	pic 125, 1
	ex1 51, 19, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 19
	sdf -1, -1
	msg 39, -1, 1
	pic 125, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 20
@if-response = 20
	sdf -1, -1
	msg -1, -1, -1
	pic 5, 4
	ex1 6, 22, -1
	ex2 -161, -1, -1
	goto 21
@disp-msg = 21
	sdf -1, -1
	msg 45, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 22
	sdf 7, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 23
@once-dlog = 23
	sdf -1, -1
	msg 46, -1, 1
	pic 125, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 29
@damage = 24
	sdf -1, -1
	msg 52, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 0, 4, -1
	goto 25
@status = 25
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 8, 0, 7
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 26
	sdf 7, 6
	msg 53, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@button-generic = 27
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 35, 11, -1
	ex2 6, 2, -1
	goto -1
@nop = 28
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-event = 29
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, -1, -1
	ex2 -1, -1, -1
	goto -1
@button-generic = 30
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 16, 2, -1
	ex2 9, 3, -1
	goto -1
