@once-dlog = 0
	sdf -1, -1
	msg 0, -1, 1
	pic 229, 1
	ex1 11, 1, -1
	ex2 12, 2, -1
	goto -1
@once-give-spec-item = 1
	sdf 13, 0
	msg 6, -1, -1
	pic 0, 4
	ex1 3, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 2
	sdf 13, 0
	msg 7, -1, -1
	pic 0, 4
	ex1 4, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 3
	sdf -1, -1
	msg 8, -1, 1
	pic 106, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 4
	sdf -1, -1
	msg 14, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 5
	sdf -1, -1
	msg 15, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@button-generic = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 30, 45, -1
	ex2 11, 0, -1
	goto -1
@once-trap = 7
	sdf 13, 1
	msg -1, -1, -1
	pic 27, 4
	ex1 1, 2, -1
	ex2 40, -1, -1
	goto -1
@once-town-encounter = 8
	sdf 13, 2
	msg 16, -1, -1
	pic 0, 4
	ex1 2, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 9
	sdf -1, -1
	msg 17, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 10
	sdf -1, -1
	msg 18, 19, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 11
	sdf -1, -1
	msg 20, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 12
	sdf 13, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 13
@damage = 13
	sdf -1, -1
	msg 21, -1, -1
	pic 0, 4
	ex1 6, 7, -1
	ex2 1, 1, -1
	goto -1
@once-set-sdf = 14
	sdf 13, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 15
@damage = 15
	sdf -1, -1
	msg 22, -1, -1
	pic 0, 4
	ex1 2, 2, -1
	ex2 2, 0, -1
	goto -1
@once-set-sdf = 16
	sdf 13, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 15
@once-set-sdf = 17
	sdf 13, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 15
@once-disp-msg = 18
	sdf 13, 7
	msg 23, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 19
	sdf 13, 8
	msg 24, -1, 1
	pic 135, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-rand = 20
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 8, 21, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 21
	sdf -1, -1
	msg 30, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 22
	sdf -1, -1
	msg 31, 32, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 23
	sdf 13, 9
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 22
@if-sdf = 25
	sdf 13, 9
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 26
@if-rand = 26
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 25, 27, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 27
	sdf -1, -1
	msg 33, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 28
	sdf 13, 9
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 29
@if-rand = 29
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 25, 30, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 30
	sdf -1, -1
	msg 34, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 31
	sdf -1, -1
	msg 35, -1, 1
	pic 130, 1
	ex1 34, 32, -1
	ex2 -1, -1, -1
	goto -1
@if-statistic = 32
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 11, 34, -1
	ex2 11, 0, -1
	goto 33
@disp-msg = 33
	sdf -1, -1
	msg 41, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@alchemy = 34
	sdf -1, -1
	msg 42, 43, -1
	pic 0, 4
	ex1 4, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 35
	sdf -1, -1
	msg 44, -1, 1
	pic 130, 1
	ex1 34, 36, -1
	ex2 -1, -1, -1
	goto -1
@if-statistic = 36
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 12, 37, -1
	ex2 11, 0, -1
	goto 33
@xp = 37
	sdf -1, -1
	msg 50, 51, -1
	pic 0, 4
	ex1 10, 1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 38
	sdf -1, -1
	msg 52, -1, 1
	pic 130, 1
	ex1 34, 39, -1
	ex2 -1, -1, -1
	goto -1
@if-statistic = 39
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 40, -1
	ex2 11, 0, -1
	goto 33
@alchemy = 40
	sdf -1, -1
	msg 58, 59, -1
	pic 0, 4
	ex1 5, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 41
	sdf 13, 9
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 42
@if-item-class-at = 42
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 43, 2, -1
	ex2 1, 43, 0
	goto -1
@set-sdf = 43
	sdf 13, 9
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 44
@once-give-spec-item = 44
	sdf -1, -1
	msg 60, 61, -1
	pic 0, 4
	ex1 10, 0, -1
	ex2 -1, -1, -1
	goto -1
@stair = 45
	sdf -1, -1
	msg 62, -1, -1
	pic 0, 4
	ex1 5, 40, -1
	ex2 15, -1, -1
	goto -1
@button-generic = 46
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 45, 2, -1
	ex2 14, 1, -1
	goto -1
