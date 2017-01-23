@once-dlog = 0
	sdf -1, -1
	msg 0, -1, 1
	pic 143, 1
	ex1 33, 1, -1
	ex2 -1, -1, -1
	goto -1
@rest = 1
	sdf -1, -1
	msg 6, -1, -1
	pic 0, 4
	ex1 300, 200, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 2
	sdf 0, 3
	msg 7, -1, 1
	pic 189, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@inc-sdf = 3
	sdf 0, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 0, -1
	ex2 -1, -1, -1
	goto 4
@block-move = 4
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto 5
@if-sdf = 5
	sdf 0, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 3, 6, -1
	ex2 -1, -1, -1
	goto 7
@disp-msg = 6
	sdf -1, -1
	msg 13, 14, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 7
	sdf 0, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 8, -1
	ex2 -1, -1, -1
	goto 9
@once-dlog = 8
	sdf -1, -1
	msg 15, -1, 1
	pic 30, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 9
	sdf -1, -1
	msg 21, -1, 1
	pic 30, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 10
	sdf 0, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, 3, -1
	goto -1
@if-sdf = 11
	sdf 0, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, 3, -1
	goto -1
@once-dlog = 12
	sdf -1, -1
	msg 27, -1, 1
	pic 11, 4
	ex1 36, 13, -1
	ex2 -1, -1, -1
	goto -1
@flip-sdf = 13
	sdf 0, 1
	msg 33, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 14
	sdf -1, -1
	msg 27, -1, 1
	pic 11, 4
	ex1 36, 15, -1
	ex2 -1, -1, -1
	goto -1
@flip-sdf = 15
	sdf 0, 2
	msg 33, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 16
	sdf 0, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 17, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 17
	sdf 0, 5
	msg 34, 35, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 18
	sdf 0, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 19, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 19
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 34, 19, -1
	ex2 161, -1, -1
	goto 20
@once-dlog = 20
	sdf 0, 6
	msg 36, -1, 1
	pic 11, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@town-visible = 21
	sdf -1, -1
	msg 42, 43, -1
	pic 0, 4
	ex1 3, 1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 22
	sdf -1, -1
	msg 44, 45, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 23
	sdf 0, 7
	msg 46, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 24
	sdf 100, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 25, -1
	ex2 -1, -1, -1
	goto 26
@disp-msg = 25
	sdf -1, -1
	msg 47, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 26
	sdf 0, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 27, -1
	ex2 -1, -1, -1
	goto 28
@disp-msg = 27
	sdf -1, -1
	msg 48, 49, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-item-class = 28
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 45, 29, -1
	ex2 0, -1, -1
	goto -1
@disp-msg = 29
	sdf -1, -1
	msg 50, 51, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 30
	sdf 0, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 31, -1
	ex2 -1, -1, -1
	goto 32
@disp-msg = 31
	sdf -1, -1
	msg 52, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-item-class = 32
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 45, 33, -1
	ex2 1, -1, -1
	goto -1
@once-dlog = 33
	sdf -1, -1
	msg 53, -1, 1
	pic 5, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 34
@set-sdf = 34
	sdf 0, 4
	msg 59, 60, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 41
@if-sdf = 35
	sdf 100, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 36, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 36
	sdf 0, 8
	msg 61, -1, 1
	pic 30, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@button-generic = 37
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 2, -1
	ex2 2, 1, -1
	goto -1
@button-generic = 38
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 30, 2, -1
	ex2 2, 1, -1
	goto -1
@button-generic = 39
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 30, 12, -1
	ex2 2, 1, -1
	goto -1
@button-generic = 40
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 30, 23, -1
	ex2 2, 1, -1
	goto -1
@nuke-monsts = 41
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 188, -1, -1
	ex2 -1, -1, -1
	goto -1
