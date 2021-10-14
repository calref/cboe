@once-dlog = 0
	sdf 1, 0
	msg 0, -1, 1
	pic 189, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 3, 2, -1
	ex2 -1, -1, -1
	goto 34
@once-dlog = 2
	sdf -1, -1
	msg 6, -1, 0
	pic 150, 1
	ex1 3, -1, -1
	ex2 2, 3, -1
	goto -1
@once-give-spec-item = 3
	sdf -1, -1
	msg 12, -1, -1
	pic 0, 4
	ex1 3, 1, -1
	ex2 -1, -1, -1
	goto 4
@set-sdf = 4
	sdf 1, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 36
@once-disp-msg = 5
	sdf 1, 2
	msg 13, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 6
	sdf -1, -1
	msg 14, -1, 1
	pic 130, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 7
	sdf -1, -1
	msg 20, -1, 1
	pic 130, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-trap = 8
	sdf 1, 3
	msg -1, -1, -1
	pic 27, 4
	ex1 0, 2, -1
	ex2 0, -1, -1
	goto -1
@once-trap = 9
	sdf 1, 4
	msg 26, -1, -1
	pic 27, 4
	ex1 8, 0, -1
	ex2 20, -1, -1
	goto -1
@once-dlog = 10
	sdf -1, -1
	msg 27, -1, 1
	pic 138, 1
	ex1 41, 11, -1
	ex2 -1, -1, -1
	goto -1
@if-gold = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 100, 13, -1
	ex2 1, -1, -1
	goto 12
@disp-msg = 12
	sdf -1, -1
	msg 33, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 13
	sdf 1, 5
	msg 34, -1, -1
	pic 0, 4
	ex1 1, 0, -1
	ex2 -1, -1, -1
	goto 35
@once-dlog = 14
	sdf -1, -1
	msg 35, -1, 1
	pic 158, 1
	ex1 37, 15, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 15
	sdf 1, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 17, -1
	ex2 -1, -1, -1
	goto 16
@disp-msg = 16
	sdf -1, -1
	msg 41, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 17
	sdf 1, 5
	msg 42, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto 18
@hp = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 100, 0, -1
	ex2 -1, -1, -1
	goto 19
@status = 19
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 7, 0, 9
	ex2 -1, -1, -1
	goto 20
@status = 20
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 8, 0, 7
	ex2 -1, -1, -1
	goto 21
@status = 21
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 5000, 0, 12
	ex2 -1, -1, -1
	goto -1
@if-sdf = 22
	sdf 100, 8
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 23, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 23
	sdf 1, 6
	msg 43, 44, -1
	pic 0, 4
	ex1 5, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-trap = 24
	sdf 1, 7
	msg -1, -1, -1
	pic 27, 4
	ex1 1, 2, -1
	ex2 40, -1, -1
	goto -1
@disp-msg = 25
	sdf -1, -1
	msg 45, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 26
	sdf 101, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 27, -1
	ex2 -1, -1, -1
	goto -1
@town-attitude = 27
	sdf -1, -1
	msg 46, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, -1, -1
	goto -1
@if-sdf = 28
	sdf 101, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 29, -1
	ex2 -1, -1, -1
	goto 30
@disp-msg = 29
	sdf -1, -1
	msg 47, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 30
	sdf -1, -1
	msg 54, -1, 0
	pic 64, 1
	ex1 3, 31, -1
	ex2 2, 32, -1
	goto -1
@block-move = 31
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 32
	sdf -1, -1
	msg 48, -1, 1
	pic 64, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 33
@end-scen = 33
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 34
	sdf -1, -1
	msg 60, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@gold = 35
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 100, 1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 36
	sdf 1, 8
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-boat = 98
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 28
@if-boat = 99
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 0
