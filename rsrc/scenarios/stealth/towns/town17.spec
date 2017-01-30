@once-dlog = 0
	sdf 17, 0
	msg 0, -1, 1
	pic 1000, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 1
@hp = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 200, 0, -1
	ex2 -1, -1, -1
	goto 2
@sp = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 100, 0, -1
	ex2 -1, -1, -1
	goto 3
@start-timer-town = 3
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 4, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 4
	sdf -1, -1
	msg 6, 7, -1
	pic 0, 4
	ex1 59, 61, -1
	ex2 170, -1, -1
	goto -1
@lever-generic = 5
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 6, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 6
	sdf -1, -1
	msg 8, -1, -1
	pic 0, 4
	ex1 59, 53, -1
	ex2 139, 170, -1
	goto -1
@lever-generic = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 8, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 8
	sdf -1, -1
	msg 8, -1, -1
	pic 0, 4
	ex1 59, 57, -1
	ex2 139, 170, -1
	goto -1
@lever-generic = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, 10, -1
	ex2 -1, -1, -1
	goto -1
@swap-ter = 10
	sdf -1, -1
	msg 8, -1, -1
	pic 0, 4
	ex1 59, 61, -1
	ex2 139, 170, -1
	goto -1
@once-dlog = 11
	sdf 17, 1
	msg 9, -1, 1
	pic 141, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 12
	sdf -1, -1
	msg 15, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 13
	sdf 17, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 14
@once-dlog = 14
	sdf -1, -1
	msg 16, -1, 1
	pic 16, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 15
@nuke-monsts = 15
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 35, -1, -1
	ex2 -1, -1, -1
	goto 16
@start-timer-town = 16
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 300, 17, -1
	ex2 -1, -1, -1
	goto 18
@disp-msg = 17
	sdf -1, -1
	msg 22, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@start-timer-town = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 500, 19, -1
	ex2 -1, -1, -1
	goto 20
@disp-msg = 19
	sdf -1, -1
	msg 23, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@start-timer-town = 20
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 650, 21, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 21
	sdf 17, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 1, 22, -1
	goto 52
@once-dlog = 22
	sdf -1, -1
	msg 24, -1, 1
	pic 16, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 23
@death = 23
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 24
	sdf -1, -1
	msg 30, 31, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-disp-msg = 25
	sdf 17, 5
	msg 32, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-ter = 26
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 24, 62, -1
	ex2 170, 27, -1
	goto -1
@change-ter = 27
	sdf -1, -1
	msg 33, -1, -1
	pic 0, 4
	ex1 24, 62, -1
	ex2 122, -1, -1
	goto -1
@if-ter = 28
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 26, 54, -1
	ex2 122, 29, -1
	goto -1
@change-ter = 29
	sdf -1, -1
	msg 34, -1, -1
	pic 0, 4
	ex1 26, 54, -1
	ex2 170, -1, -1
	goto -1
@block-move = 30
	sdf -1, -1
	msg 35, 36, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-set-sdf = 31
	sdf 17, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 32
@play-sound = 32
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 5, -1, -1
	ex2 -1, -1, -1
	goto 33
@once-dlog = 33
	sdf -1, -1
	msg 37, -1, 1
	pic 95, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 34
	sdf 17, 7
	msg 43, 44, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 35
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 61, 33, -1
	ex2 -1, -1, -1
	goto -1
@portal-generic = 36
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 42, 22, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 37
	sdf 17, 8
	msg 45, -1, 1
	pic 1000, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 38
@once-town-encounter = 38
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 39
@if-spec-item = 39
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 16, 40, -1
	ex2 -1, -1, -1
	goto 41
@once-dlog = 40
	sdf -1, -1
	msg 51, -1, 0
	pic 1000, 3
	ex1 3, 41, -1
	ex2 2, 42, -1
	goto -1
@once-town-encounter = 41
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 42
	sdf -1, -1
	msg 57, -1, 1
	pic 19, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 43
	sdf -1, -1
	msg 63, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 44
	sdf -1, -1
	msg 64, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-context = 45
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 16, 108, 46
	ex2 -1, -1, -1
	goto -1
@disp-msg = 46
	sdf -1, -1
	msg 65, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-statistic = 47
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 13, 48, -1
	ex2 11, 0, -1
	goto 49
@spell-priest = 48
	sdf -1, -1
	msg 66, 67, -1
	pic 0, 4
	ex1 54, 0, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 49
	sdf -1, -1
	msg 68, 69, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-statistic = 50
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 15, 51, -1
	ex2 11, 0, -1
	goto 49
@spell-priest = 51
	sdf -1, -1
	msg 70, 71, -1
	pic 0, 4
	ex1 61, 0, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 52
	sdf -1, -1
	msg 72, 73, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 53
@death = 53
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto -1
@if-ter = 54
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 8, 1, -1
	ex2 128, 55, -1
	goto -1
@if-spec-item = 55
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 17, 56, -1
	ex2 -1, -1, -1
	goto 57
@change-ter = 56
	sdf -1, -1
	msg 74, -1, -1
	pic 0, 4
	ex1 8, 1, -1
	ex2 125, -1, -1
	goto -1
@disp-msg = 57
	sdf -1, -1
	msg 75, 76, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 58
	sdf -1, -1
	msg 77, -1, 1
	pic 1000, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 59
@set-sdf = 59
	sdf 17, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 60
@xp = 60
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 100, 0, -1
	ex2 -1, -1, -1
	goto 61
@once-give-spec-item = 61
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 17, 0, -1
	ex2 -1, -1, -1
	goto -1
@button-generic = 62
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 26, 29, -1
	ex2 19, 1, -1
	goto -1
