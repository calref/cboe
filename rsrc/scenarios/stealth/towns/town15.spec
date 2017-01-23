@block-move = 0
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@once-dlog = 1
	sdf 15, 0
	msg 0, -1, 1
	pic 194, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 2
	sdf -1, -1
	msg 6, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-ter = 3
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 9, 37, -1
	ex2 131, 4, -1
	goto -1
@if-sdf = 4
	sdf 15, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, 64, -1
	goto -1
@change-ter = 5
	sdf -1, -1
	msg 7, -1, -1
	pic 0, 4
	ex1 9, 37, -1
	ex2 130, -1, -1
	goto -1
@if-sdf = 6
	sdf 15, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, 7, -1
	goto -1
@if-ter = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 9, 30, -1
	ex2 130, 8, -1
	goto -1
@change-ter = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 9, 30, -1
	ex2 131, -1, -1
	goto 9
@change-ter = 9
	sdf -1, -1
	msg 8, -1, -1
	pic 0, 4
	ex1 9, 37, -1
	ex2 131, -1, -1
	goto -1
@once-dlog = 10
	sdf 15, 2
	msg 10, -1, 1
	pic 90, 1
	ex1 29, 12, -1
	ex2 -1, -1, -1
	goto 11
@block-move = 11
	sdf -1, -1
	msg 9, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 12
	sdf -1, -1
	msg 16, -1, 1
	pic 12, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 13
@change-ter = 13
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 9, 37, -1
	ex2 130, -1, -1
	goto 14
@town-attitude = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto 15
@once-set-sdf = 15
	sdf 15, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 26
@if-ter = 16
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 13, 32, -1
	ex2 122, 17, -1
	goto -1
@change-ter = 17
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 13, 32, -1
	ex2 138, -1, -1
	goto 18
@change-ter = 18
	sdf -1, -1
	msg 22, -1, -1
	pic 0, 4
	ex1 13, 35, -1
	ex2 138, -1, -1
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
	msg 23, -1, -1
	pic 0, 4
	ex1 9, 37, -1
	ex2 130, 131, -1
	goto 62
@once-dlog = 21
	sdf -1, -1
	msg 24, -1, 1
	pic 158, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 22
	sdf 15, 3
	msg 30, 31, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 23
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@once-dlog = 24
	sdf 15, 4
	msg 32, -1, 1
	pic 25, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 25
	sdf -1, -1
	msg 38, 39, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 26
	sdf 101, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 27
	sdf 15, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, 28, -1
	goto -1
@change-ter = 28
	sdf -1, -1
	msg 40, 41, -1
	pic 0, 4
	ex1 29, 10, -1
	ex2 170, -1, -1
	goto 29
@set-sdf = 29
	sdf 15, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 37
@once = 30
	sdf 15, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 31
@if-sdf = 31
	sdf 15, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 32, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 32
	sdf 15, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 33
@rect-place-field = 33
	sdf 100, 32
	msg -1, -1, -1
	pic 0, 4
	ex1 9, 26, -1
	ex2 16, 29, -1
	goto 34
@change-ter = 34
	sdf -1, -1
	msg 42, 43, -1
	pic 0, 4
	ex1 25, 11, -1
	ex2 129, -1, -1
	goto 35
@once-town-encounter = 35
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 38
@once-dlog = 36
	sdf 15, 7
	msg 44, -1, 1
	pic 90, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 37
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 30, 14, -1
	ex2 128, -1, -1
	goto -1
@change-ter = 38
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 30, 14, -1
	ex2 129, -1, -1
	goto -1
@change-ter = 39
	sdf -1, -1
	msg 50, 51, -1
	pic 0, 4
	ex1 10, 19, -1
	ex2 97, -1, -1
	goto 41
@once-set-sdf = 40
	sdf 15, 8
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 39
@change-ter = 41
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 11, 19, -1
	ex2 97, -1, -1
	goto -1
@start-timer-town = 42
	sdf -1, -1
	msg 52, 53, -1
	pic 0, 4
	ex1 3, 44, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 43
	sdf 15, 9
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 42
@anim-explode = 44
	sdf -1, -1
	msg 54, -1, -1
	pic 2, 4
	ex1 27, 1, -1
	ex2 0, -1, -1
	goto 45
@explode-space = 45
	sdf -1, -1
	msg -1, -1, -1
	pic 2, 4
	ex1 27, 1, -1
	ex2 1000, 1, -1
	goto 46
@explode-space = 46
	sdf -1, -1
	msg -1, -1, -1
	pic 4, 4
	ex1 27, 1, -1
	ex2 40, 1, -1
	goto 47
@change-ter = 47
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 27, 1, -1
	ex2 0, -1, -1
	goto 48
@change-ter = 48
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 20, 6, -1
	ex2 84, -1, -1
	goto 65
@once-disp-msg = 49
	sdf 120, 0
	msg 55, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 50
	sdf 120, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 51
@change-ter = 51
	sdf -1, -1
	msg 56, -1, -1
	pic 0, 4
	ex1 5, 4, -1
	ex2 170, -1, -1
	goto 52
@change-ter = 52
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 6, 3, -1
	ex2 122, -1, -1
	goto 53
@change-ter = 53
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 4, -1
	ex2 123, -1, -1
	goto 54
@change-ter = 54
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 7, 7, -1
	ex2 129, -1, -1
	goto -1
@lever-generic = 55
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 56, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 56
	sdf -1, -1
	msg 57, -1, -1
	pic 0, 4
	ex1 16, 35, -1
	ex2 130, 131, -1
	goto -1
@once-dlog = 57
	sdf 120, 2
	msg 58, -1, 1
	pic 16, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 58
	sdf -1, -1
	msg 64, -1, 1
	pic 16, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 59
@set-sdf = 59
	sdf 101, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 60
@set-sdf = 60
	sdf 101, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 61
@xp = 61
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 20, 0, -1
	ex2 -1, -1, -1
	goto 66
@set-sdf = 62
	sdf 101, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 63
	sdf 101, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-ter = 64
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 9, 30, -1
	ex2 130, 5, -1
	goto -1
@explode-space = 65
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 27, 1, -1
	ex2 1000, 1, -1
	goto -1
@set-sdf = 66
	sdf 15, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
