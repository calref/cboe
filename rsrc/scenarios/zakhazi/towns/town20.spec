@if-ter = 0
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 45, 4, -1
	ex2 130, 5, -1
	goto -1
@change-ter = 1
	sdf -1, -1
	msg 0, 1, -1
	pic 0, 4
	ex1 45, 4, -1
	ex2 131, -1, -1
	goto -1
@once-dlog = 2
	sdf -1, -1
	msg 2, -1, 1
	pic 124, 1
	ex1 8, 3, -1
	ex2 -1, -1, -1
	goto 55
@block-move = 3
	sdf -1, -1
	msg 8, 9, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto 4
@damage = 4
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 8, 9, -1
	ex2 1, 1, -1
	goto -1
@if-sdf = 5
	sdf 20, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 6
	sdf 20, 1
	msg 10, 11, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 7
	sdf 20, 2
	msg 12, -1, 1
	pic 12, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 8
	sdf 20, 3
	msg 18, 19, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 9
	sdf -1, -1
	msg 20, -1, 1
	pic 124, 1
	ex1 8, 11, -1
	ex2 -1, -1, -1
	goto 10
@block-move = 10
	sdf -1, -1
	msg 26, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@block-move = 11
	sdf -1, -1
	msg 27, 28, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto 12
@damage = 12
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 20, 10, -1
	ex2 0, 0, -1
	goto 13
@damage = 13
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 20, 10, -1
	ex2 0, 3, -1
	goto 14
@damage = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 20, 10, -1
	ex2 0, 4, -1
	goto -1
@if-statistic = 15
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 14, 17, -1
	ex2 11, 0, -1
	goto 16
@disp-msg = 16
	sdf -1, -1
	msg 29, 30, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@spell-priest = 17
	sdf -1, -1
	msg 31, 32, -1
	pic 0, 4
	ex1 56, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-trap = 18
	sdf 140, 0
	msg -1, -1, -1
	pic 27, 4
	ex1 0, 3, -1
	ex2 20, -1, -1
	goto -1
@once-trap = 19
	sdf 140, 1
	msg -1, -1, -1
	pic 27, 4
	ex1 0, 3, -1
	ex2 13, -1, -1
	goto -1
@once-trap = 20
	sdf 140, 2
	msg -1, -1, -1
	pic 27, 4
	ex1 0, 3, -1
	ex2 25, -1, -1
	goto -1
@once-trap = 21
	sdf 140, 3
	msg -1, -1, -1
	pic 27, 4
	ex1 0, 3, -1
	ex2 20, -1, -1
	goto -1
@once-trap = 22
	sdf 140, 4
	msg -1, -1, -1
	pic 27, 4
	ex1 0, 3, -1
	ex2 15, -1, -1
	goto -1
@once-trap = 23
	sdf 140, 5
	msg -1, -1, -1
	pic 27, 4
	ex1 0, 3, -1
	ex2 13, -1, -1
	goto -1
@once-trap = 24
	sdf 140, 6
	msg -1, -1, -1
	pic 27, 4
	ex1 0, 3, -1
	ex2 14, -1, -1
	goto -1
@once-dlog = 25
	sdf 20, 4
	msg 33, -1, 1
	pic 133, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 26
@once-town-encounter = 26
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 27
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 13, 45, -1
	ex2 131, -1, -1
	goto -1
@change-ter = 28
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 25, 45, -1
	ex2 131, -1, -1
	goto -1
@change-ter = 29
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 38, 44, -1
	ex2 131, -1, -1
	goto -1
@once = 30
	sdf 20, 5
	msg -1, 0, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 31
@once-dlog = 31
	sdf -1, -1
	msg 40, -1, 0
	pic 133, 1
	ex1 3, 32, -1
	ex2 2, 33, -1
	goto -1
@block-move = 32
	sdf -1, -1
	msg 39, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-set-sdf = 33
	sdf 20, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 34
@disp-msg = 34
	sdf -1, -1
	msg 46, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 35
	sdf 20, 6
	msg 47, -1, 1
	pic 232, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 36
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 54, 55, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 37
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 14, 45, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 38
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 44, -1
	ex2 -1, -1, -1
	goto -1
@once-town-encounter = 39
	sdf 20, 7
	msg -1, -1, -1
	pic 0, 4
	ex1 2, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-town-encounter = 40
	sdf 20, 8
	msg -1, -1, -1
	pic 0, 4
	ex1 3, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 41
	sdf 20, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 42, -1
	ex2 -1, -1, -1
	goto 43
@disp-msg = 42
	sdf -1, -1
	msg 53, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 43
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 3, 44, -1
	ex2 -1, -1, -1
	goto 48
@once-dlog = 44
	sdf -1, -1
	msg 54, -1, 1
	pic 12, 4
	ex1 39, 46, -1
	ex2 -1, -1, -1
	goto 45
@disp-msg = 45
	sdf -1, -1
	msg 60, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 46
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 3, 1, -1
	ex2 -1, -1, -1
	goto 47
@set-sdf = 47
	sdf 20, 0
	msg 61, 75, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 48
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 49, -1
	ex2 -1, -1, -1
	goto 51
@once-dlog = 49
	sdf -1, -1
	msg 62, -1, 1
	pic 12, 4
	ex1 39, 50, -1
	ex2 -1, -1, -1
	goto 45
@once-give-spec-item = 50
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto 47
@if-spec-item = 51
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 52, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 52
	sdf -1, -1
	msg 68, -1, 1
	pic 12, 4
	ex1 39, 53, -1
	ex2 -1, -1, -1
	goto 45
@once-give-spec-item = 53
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 1, -1
	ex2 -1, -1, -1
	goto 47
@disp-msg = 54
	sdf -1, -1
	msg 74, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 55
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@change-ter = 56
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 45, 4, -1
	ex2 131, -1, -1
	goto -1
