@once-dlog = 0
	sdf -1, -1
	msg 25, -1, 1
	pic 140, 1
	ex1 51, 24, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 1
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@once-town-encounter = 2
	sdf 8, 1
	msg 1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 3
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 48, 19, -1
	ex2 139, -1, -1
	goto 4
@change-ter = 4
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 51, 22, -1
	ex2 140, -1, -1
	goto -1
@if-sdf-eq = 5
	sdf 8, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 6, -1
	ex2 -1, -1, -1
	goto -1
@move-party = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 47, 18, -1
	ex2 0, -1, -1
	goto -1
@disp-sm-msg = 7
	sdf -1, -1
	msg 2, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 8
@set-sdf = 8
	sdf 8, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 52, 24, -1
	ex2 148, -1, -1
	goto -1
@change-ter = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 52, 24, -1
	ex2 147, -1, -1
	goto -1
@if-sdf = 11
	sdf 8, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 12, -1
	ex2 -1, -1, -1
	goto 13
@disp-msg = 12
	sdf -1, -1
	msg 3, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 13
	sdf 8, 4
	msg 4, -1, 3
	pic 169, 1
	ex1 -1, -1, -1
	ex2 -1, 14, -1
	goto -1
@set-sdf = 14
	sdf 8, 0
	msg 10, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 15
@town-attitude = 15
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, -1, -1
	goto -1
@once-trap = 16
	sdf 8, 6
	msg -1, -1, -1
	pic 27, 4
	ex1 8, 0, -1
	ex2 60, -1, -1
	goto -1
@once-dlog = 17
	sdf -1, -1
	msg 11, -1, 1
	pic 130, 1
	ex1 34, 18, -1
	ex2 -1, -1, -1
	goto -1
@select-pc = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 75, 4
	ex1 0, 1, -1
	ex2 1, -1, -1
	goto 19
@statistic = 19
	sdf -1, -1
	msg 17, 18, -1
	pic 100, 4
	ex1 3, 1, -1
	ex2 2, -1, -1
	goto -1
@alchemy = 20
	sdf -1, -1
	msg 19, -1, -1
	pic 0, 4
	ex1 14, -1, -1
	ex2 -1, -1, -1
	goto -1
@spell-priest = 21
	sdf -1, -1
	msg 20, 21, -1
	pic 0, 4
	ex1 60, 0, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 22
	sdf -1, -1
	msg 22, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 23
	sdf 8, 7
	msg 23, 24, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@select-pc = 24
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto 25
@death = 25
	sdf -1, -1
	msg 31, 32, -1
	pic 0, 4
	ex1 0, 1, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 26
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 37, 27, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 27
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 48, 7, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 28
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 47, 37, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 29
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 6, 56, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 30
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 6, 27, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 31
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 27, 28, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 32
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 24, 57, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 33
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 24, 47, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 34
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 24, 37, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 35
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 41, 37, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 36
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 48, 13, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 37
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 56, 28, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 38
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 41, 47, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 39
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 41, 57, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 40
	sdf 8, 8
	msg 33, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 41
	sdf 8, 9
	msg 34, -1, 1
	pic 62, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 42
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@if-sdf = 43
	sdf 8, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 44, -1
	ex2 -1, -1, -1
	goto -1
@town-attitude = 44
	sdf -1, -1
	msg 40, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, -1, -1
	goto -1
@town-attitude = 45
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, -1, -1
	goto -1
@xp = 46
	sdf -1, -1
	msg 41, -1, -1
	pic 0, 4
	ex1 20, 0, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 47
	sdf 8, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 45, -1
	ex2 -1, -1, -1
	goto 43
@if-sdf = 48
	sdf 110, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 49, -1
	ex2 -1, -1, -1
	goto 54
@disp-msg = 49
	sdf -1, -1
	msg 42, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 50
	sdf -1, -1
	msg 43, -1, 1
	pic 22, 4
	ex1 8, 51, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 51
	sdf -1, -1
	msg 49, 50, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 52
@stair = 52
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 5, 41, -1
	ex2 10, 1, -1
	goto 53
@if-spec-item = 54
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 49, -1
	ex2 -1, -1, -1
	goto 50
